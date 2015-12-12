About this Project
------------------
Project home page: https://github.com/generic-electric-unicycle/documentation/wiki

Disclaimer
==========

This is a **DEVELOPER PREVIEW** intended for developers and testers only.
**DO NOT RUN THIS CODE FOR NORMAL USE. DO NOT REQUEST ANY HELP OR SUPPORT.**

We are **NOT** responsible for any damage this software may cause. 

Build Instructions
-------------------------
#### Using GCC-Sources

1. Get gcc-arm toolchain from https://launchpad.net/gcc-arm-embedded
   (tested with gcc-arm-none-eabi-4_9-2015q2-20150609-linux)
2. Unpack to a directory of your choice (but look at the relative reference)
3. Make sure that the following path relative to the git root directory
   exists and contains the gcc-arm toolchain:
   ../gcc/arm-none-eabi
   NOTE: on ../gcc folder you should have the folders: arm-none-eabi; bin; lib; share
   (alternatively you can make a local change to the makefile)
4. Rename Makefile.src -> Makefile
5. In the repository root directory type 'make'



#### Using Launchpad-PPA (Ubuntu)

1. `$ sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded`
2. `$ sudo apt-get update`
3. `$ sudo apt-get install gcc-arm-none-eabi`
4. Navigate to the repository root directory
5. Rename Makefile to use correct path / libs: `$ mv Makefile.ppa Makefile`
6. Build the Firmware: `$ make`


Flash Firmware to Controller
----------------------------
1. Navigate to the `tools` folder in the repository
2. Edit the file: `flash`
3. Select matching "Driver" by uncomment one Option/Line
4. Navigate back to the repository root directory and type `$ make flash`
5. Done


Current Features
----------------
1. None


Developement Team
-----------------
This project is open source and maintained by a team of volunteers.
To contribute, you can send a pull request on Github. You can also join the discussion on [electricunicycle.org](http://forum.electricunicycle.org/topic/1109-firmware).


License
-------
This project is licensed under the General Public License Version 3.0 (GPL v3).
