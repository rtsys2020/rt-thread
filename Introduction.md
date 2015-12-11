# Introduction #

The Real-Time Thread(RT-Thread) Operating System is an open source real-time operating system developed by the RT-Thread Studio based in China, after five years' fully concentrated development. It is aimed to change the current situation in China that there is no well used open souce real-time operating system in the microcontroller area. It is a real-time operating system not only on the open source level but also on the commercial standard level. Having been used by more than ten companies in China, it proved to be an operating system running stably for a long time.

It has been one year since our RT-Thread official version 0.2.4 has been released. The version 0.3.0 branch is officially released. We have received lot of feedbacks from our customers, and have been fixing bugs as well as making big progress.

## RT-Thread Kernel ##

  * Object oriented real-time core (while remaining the elegant and flexible style of C Programming Language);
  * 32 or 256 priority scheduling multi-thread scheduling; Using the round-robin policy ensures that all threads having the same priority level will be scheduled equally;
  * Synchronization of threads: semaphore and mutual exclusion semaphore (mutex) to prevent priority inversion;
  * Complete and efficient support for communication between threads, including mailbox, message queues, event flag;
  * Static memory management supports thread suspend/resume when it allocates/frees a memory block and thread-safe dynamic heap management;
  * A device driver framework to provide standard interface to high level application;

# FinSH shell #
  * Command line that accepts C-like expression;
  * Access system core functions directly via command line like C Programming Language  grammar;
  * Access system global variables directly via command line like C Programming Language  grammar;
  * Command history records and automatic complete for the Command Prompt;

## Device File System ##
  * Virtual File System optimised for small device
  * POSIX style API;
  * Support the different implementation of file systems
  * Wrapper for ELM Chan's FatFs filesystem.

## LwIP, a lightweight TCP/IP protocol stack ##
  * Standard BSD Socket interface;
  * IP, ICMP, UDP, TCP supported
  * DNS, DHCP, PPP supported
  * TFTP、HTTP、FTP supported (refer to the netutil component)

## RT-Thread/GUI ##
  * Integrated with RT-Thread;
  * Multi-Thread supported;
  * Multi-Window supported;
  * Rich Widgets such as: label, button, checkbox, radiobox, etc.
  * Client/Server Architecture;
  * Client: Workbench/View/Window Architecture;
  * Chinese GB2312 display.
![http://www.rt-thread.org/rt-thread/gui.png](http://www.rt-thread.org/rt-thread/gui.png)