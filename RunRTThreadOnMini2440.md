# Howto Run RT-Thread 0.4.0 on QEMU/mini2440 #

1.Get RT-Thread RTOS source code
  * <a href='http://rt-thread.googlecode.com/files/rt-thread-0.4.0%20beta1.zip'>RT-Thread 0.4.0 beta1</a>

2. Install scons build system
  * Make sure you have installed python 2.5.x and scons 1.x or 2.x, belowing is the download URL of the software for windows users:
    1. http://www.rt-thread.org/download/python-2.5.2.msi
    1. http://www.rt-thread.org/download/scons-2.0.1.win32.exe

3. Install compiler
  * You can choose to GNU GCC or armcc as compiler.
    * If using gcc compiler, [CodeSourcery ARM EABI](http://www.codesourcery.com/sgpp/lite/arm/portal/subscription?@template=lite) toolchain is one of the choice.
    * If using armcc, Keil MDK 3.8 IDE is the choice.

> Make sure you have installed one of them.

4. Edit "bsp\mini2440\rtconfig.py" to configure devlopement environment
  * if using gcc compiler, set CROSS\_TOOL = 'gcc', else CROSS\_TOOL = 'armcc'
  * set EXEC\_PATH to your toolchains' install path, for example, my EXEC\_PATH = `E:/Program Files/CodeSourcery/Sourcery G++ Lite/bin`
```
CROSS_TOOL 	= 'gcc'

if  CROSS_TOOL == 'gcc':
	PLATFORM 	= 'gcc'
	EXEC_PATH 	= 'E:/Program Files/CodeSourcery/Sourcery G++ Lite/bin'
elif CROSS_TOOL == 'keil':
	PLATFORM 	= 'armcc'
	EXEC_PATH 	= 'E:/Keil'
```

5. Compile RT-Thread RTOS
  * open command line console, suppose you unzip RT-Thread 0.4.0 beta1 under d:\rt-thread, and install Python 2.5 in C:\Python25:
```
cd rt-thread\bsp\mini2440
set PATH=C:\python25\scripts;%PATH%
scons
```

  * ![http://www.rt-thread.org/rt-thread/scons.jpg](http://www.rt-thread.org/rt-thread/scons.jpg)

  * rtthread-mini2440.axf and rtthread-mini2440.bin would be created in "bsp/mini2440" direcotry when compiling successfully

6. Run in QEMU/mini2440 emulator
  * make sure rtthread-mini2440.axf have been created in "bsp/mini2440" direcotry
  * unzip SDCARD.zip file which located in "tools" directory to current directory
  * run run-mini2440-sdcard.bat and RT-Thread RTOS would be run

  * ![http://www.rt-thread.org/rt-thread/qemu.jpg](http://www.rt-thread.org/rt-thread/qemu.jpg)

7. Run in mini2440 dev board
  * use supervivi's command "d" to load rt-thread.bin from PC to board's SDRAM, load address should be set to 0x30000000. RT-Thread RTOS would be run when loaded sucessfully.

  * ![http://www.rt-thread.org/rt-thread/friendly.jpg](http://www.rt-thread.org/rt-thread/friendly.jpg)

8.burn rt-thread.bin to dev board's nandflash
  * install uboot on NANDFLASH
  * boot from NANDFLASH
  * using uboot's "loadb command" or "tftp" to load rt-thread.bin to 0x30000000
```
tftp 0x30000000 192.168.1.5:rtthread.bin
nand erase 0x100000 0x150000
nand write 0x30000000 0x100000 0x150000
set bootcmd 'nand read 0x30000000 0x100000 0x150000;go 0x30000000'
saveenv
```
  * then reset and boot from NANDFLASH, rt-thread would be loaded from NANDFLASH to SDRAM's
0x30000000 address by uboot and start running from there.