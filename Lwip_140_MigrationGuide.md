# lwip1.4迁移指南 #

## lwip，netif架构 ##

```
                    +-----------------------+
                    |   driver interface    |
                    |                       |
                    |   struct eth_device   |
                    | +-eth_rx_ready()      |
                +---->+-link_up()      |    |
                |   | +-link_down()    |    |
                |   +------------------|----+
+-------------+ |   +------------------|----+    +-------------------+
| driver      | |   |   ethernetif     |    |    | lwip 1.4          |
|             | |   |                  V    |    |                   |
| isr() --------+   |   rx_thread           |    | tcpip_thread      |
| rx()  <-------------- netif->input  ------------>tcpip_input       |
| tx()  <-------------- netif->output <----------- enetif_linkoutput |
|             |     |                       |    |                   |
+-------------+     +-----------------------+    +-------------------+

```

lwip 1.4的驱动架构和1.32相比并没有太大的变化，不过细节方面有所改动，各个模块之间功能划分更明确：
  * 驱动部分
    1. 驱动的输入只有入口，就是isr。isr接收到各种中断以后做相应的处理，包括接收到rx包，链路层通断消息等，然后调用ethernetif.h头文件中的各个函数来通知netif层。
    1. 驱动的另一个主要需要实现的部分是rx和tx这两个回调函数，它是提供给ethernetif来进行链路层数据收发使用。rx函数一般会被netif中的rx线程调用，然后将数据包转发给tcpip层。tx函数有可能被netif中的tx线程调用(lwip1.32的实现）或者直接被tcpip的enetif\_linkoutput调用，来进行链路层发送数据包使用。
  * ethernetif
    1. ethernetif输入部分包含一个线程，用来接收rx消息，以及其他链路层状态消息，然后反馈给lwip。输出部分由上层lwip调入netif->output函数，这里有可能作为消息传入tx线程（1.32实现）或者直接送给驱动的tx()。
    1. 相关参考 [Network interfaces management](http://lwip.wikia.com/wiki/Network_interfaces_management)
  * lwip1.4
    1. lwip1.4 初始化流程和1.32的流程方面有所区别，具体参见下一节。


### 初始化流程 ###
初始化的过程包括两个独立的部分：
  * 驱动初始化
  * lwip和netif的初始化
例如在mini2440平台上只需要分别执行连个函数即可：
rt\_hw\_dm9000\_init();
lwip\_enetif\_init();
驱动的初始化可以放在启动第一个进程之前进行，而第二个函数需要在线程上下文中执行。

#### 驱动初始化部分 ####
  * 创建eth\_device的继承结构
  * 初始化eth\_device中除了netif以外的所有域（netif结构会在ethernetif初始化结束以后填充）
  * 调用rt\_device\_register注册结构体
  * 注册中断函数
完成这几件事情以后可能驱动部分就已经开始工作了，但是这时候lwip部分还完全没有初始化。所以驱动部分还不能把消息传递给lwip/netif层。在这里因为eth\_device的netif指针仍然没有填充，而这个指针是驱动通向netif的桥梁，所以虽然此时驱动在工作，例如可能接收中断已经打开了，但是所有的包都会被丢弃，而不会传递到lwip去处理。

#### lwip和netif的初始化部分 ####
lwip初始化需要遵循一定的流程，主函数是lwip\_enetif\_init。 在RTOS的环境下需要调用tcpip\_init来初始化多线程的tcpip环境，初始化tcpip的内容就放在这个函数的参数中tcpip\_init\_done\_callback。之所以需要把这些函数都放在这个callback函数中的缘故是很多函数都必须在tcpip线程中执行，以避免并发问题，例如netif\_set\_up之类的函数。
在tcpip\_init\_done\_callback中初始化了ethernetif并注册到lwip中，以上这一部分一般来说应该是应用相关的，这里只是提供一个公共的参考流程。在初始化的最后，enetif\_init函数中先将netif->linkoutput设置成了设备驱动对应的函数，在这一点的时候实现了从netif到驱动的连接，然后又设置了dev->netif指针，于是从驱动到netif的连接也完成了，到此为止lwip初始化完成。
有几点需要注意的：
  * 在启动ethernetif的时候如果是启用了DHCP则应该调用dhcp\_start，如果启用AUTOIP则是autoip\_start，否则调用netif\_set\_up。
  * 包括像netif\_set\_up，dhcp\_start之类的函数，凡是在tcpip线程以外调用的时候都需要以msg发送给tcpip线程执行的形式来执行以避免并行问题。


## 从lwip1.32升级 ##
从lwip1.32升级到lwip1.4主要需要修改的地方有下面三个

### 驱动修改 ###
以dm9000的驱动为例，需要将eth\_device\_ready函数换成eth\_rx\_ready，这主要是一个语义上的改动。同样，驱动还可以通过调用eth\_linkup,eth\_linkdown之类的函数来通知netif相应的状态消息。
```
-		eth_device_ready(&(dm9000_device.parent));
+			eth_rx_ready(&(dm9000_device.parent));
```

在驱动初始化函数中，原先的eth\_device\_init需要删除，然后添加初始化和注册设备驱动函数。
```
 
-	eth_device_init(&(dm9000_device.parent), "e0");
+	dm9000_device.parent.parent.type = RT_Device_Class_NetIf;
+	rt_device_register(&(dm9000_device.parent.parent), "eth0", RT_DEVICE_FLAG_RDWR);
 
```

最后，添加一个函数get\_eth\_dev，这个函数是在ethernetif最后被lwip初始化的时候获得设备指针用的。对ethernetif来说必须有一个驱动与之对应，在逻辑上netif并不是驱动的一部分，而是注册在lwip中的一部分，它必须由lwip来初始化，由lwip来使用。
```
+struct eth_device * get_eth_dev(void)
+{
+	return (struct eth_device *)&dm9000_device;
+}
```

### 启动调用 ###
目前的启动调用流程逻辑很清楚，按顺序调用下面两个函数即可。
```
rt_hw_dm9000_init();  //初始化设备驱动
lwip_enetif_init();   //初始化lwip，并间接初始化ethnetif，最后完成ethernetif和驱动的连接。
```

而原先的
```
lwip_sys_init(void);
eth_system_device_init();
rt_hw_dm9000_init();
```
只有第三个函数保留，但功能上会有变化，其余两个函数不再使用。

### lwip netconn函数修改 ###
lwip1.4修改了netconn调用接口，所有函数统一返回错误值，例如：
lwip1.32中recv函数是
```
struct netbuf *
netconn_recv(struct netconn *conn)
```
到了lwip1.4中改为
```
err_t
netconn_recv(struct netconn *conn, struct netbuf **new_buf)
```
原先以返回值得到的netbuf改成通过参数返回，其他函数大同小异，可以此类推。

# 参考链接 #
[Porting for an OS 1.4.0](http://lwip.wikia.com/wiki/Porting_for_an_OS_1.4.0)

[Writing a device driver](http://lwip.wikia.com/wiki/Writing_a_device_driver)

[AUTOIP](http://lwip.wikia.com/wiki/AUTOIP)

[Network interfaces management](http://lwip.wikia.com/wiki/Network_interfaces_management)