Date: 24.04.2019

----

# Debugging

I have a goal that I want to debug under every step of the development. This is paramount to creating a solid kernel from the start. In previous iterations I had used bochs directly and later migrated to GDB when developing the kernel.

I had not managed to get debugging to work before late in the process, and it was very time consuming not beeing able to debug properly until then. So now in this iteration I want to enable debugging from start and set up my development environment around this.

## GDB

Installing GDB on WSL via standard `sudo apt-get install gdb`

For VSCode I have so far had a look at the C/C++ tools for VSCode and have not had any luck with getting that to connect to Qemu. After a tip I downloaded "Native Debug" extension (named: `webfreak.debug`) https://github.com/WebFreak001/code-debug

