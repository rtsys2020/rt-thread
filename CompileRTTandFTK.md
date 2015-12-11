Howto Compile RT-Thread && FTK for QEMU/mini2440

1.Get RT-Thread && FTK source code

  * <a href='http://rt-thread.googlecode.com'>Get RT-Thread source code from SVN server</a>
  * Get FTK source code
    * Make sure you have read <a href='http://code.google.com/p/rt-thread/source/browse/trunk/components/external/ftk/readme.txt'>readme.txt</a>and get FTK source code

2. Install scons build system

  * Make sure you have installed python 2.5.x and scons 1.x or 2.x, belowing is the download URL of the software for windows users:
    * http://www.rt-thread.org/download/python-2.5.2.msi
    * http://www.rt-thread.org/download/scons-2.0.1.win32.exe

3. Install compiler

  * You can choose to GNU GCC or armcc as compiler.
    * If using gcc compiler, CodeSourcery ARM EABI toolchai n is one of the choice.

4. Edit "bsp\mini2440\rtconfig.py" to configure devlopement environment

  * set EXEC\_PATH to your toolchains' install path, for example, my EXEC\_PATH = E:/Program Files/CodeSourcery/Sourcery G++ Lite/bin
```
      CROSS_TOOL      = 'gcc'

      if  CROSS_TOOL == 'gcc':
              PLATFORM        = 'gcc'
              EXEC_PATH       = 'E:/Program Files/CodeSourcery/Sourcery G++ Lite/bin'
```

5. Edit "bsp\mini2440\rtconfig.h" to configure building components
  * Open Macro
```
    #define RT_USING_NEWLIB
    #define RT_USING_PTHREADS
    #define RT_USING_DFS_DEVFS
    #define RT_USING_FTK
    #define DFS_USING_WORKDIR
```
  * Close Macro
```
    /* #define RT_USING_RTGUI */
```

6. Compile RT-Thread && FTK

  * open command line console, suppose your source is under d:\rt-thread, and install Python 2.5 in C:\Python25:
```
      cd rt-thread\bsp\mini2440
      set PATH=C:\python25\scripts;%PATH%
      scons
```

  * rtthread-mini2440.axf and rtthread-mini2440.bin would be created in "bsp/mini2440" direcotry when compiling successfully

7. Run in QEMU/mini2440 emulator

  * make sure rtthread-mini2440.axf have been created in "bsp/mini2440" direcotry
  * unzip SDCARD.z i p file which located in "tools" directory to current directory
  * run run-mini2440-sdcard.bat and RT-Thread && FTK would be run