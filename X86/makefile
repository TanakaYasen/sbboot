# set PATH=D:\Program Files\nasm-2.15.01;%PATH%;D:\repos\WRK1.2\tools\x86;
# also MSYS env is involved.

BASH = "D:\Program Files\SmartGit\git\git-bash.exe"
ASFLAGS = -I../common
CFLAGS = -nologo -Zl -Zp8 -Gy -cbstring -W3 -WX -GR- -GF -GS- -Gm- -Gz -Z7 -Oxs -Oy-
LFLAGS =  -nodefaultlib -machine:x86  -driver -subsystem:native,5.02 -WX -release -version:5.2 -osversion:5.2

all:
	nasm $(ASFLAGS) ../common/mini-mbr.asm -o mbr.bin
	nasm mini-head-x86.asm -o head.bin
	
	cl.exe $(CFLAGS) -c loader.c
	cl.exe $(CFLAGS) -c idt.c
	cl.exe $(CFLAGS) -c gdt.c
	cl.exe $(CFLAGS) -c vesa.c
	cl.exe $(CFLAGS) -c vga.c
	cl.exe $(CFLAGS) -c ReadHD.c
	cl.exe $(CFLAGS) -c ../common/printk.c
	nasm.exe inout.asm -fwin32

	link.exe $(LFLAGS) -entry:loader_main -base:0x100000 -map:loader.map /out:loader.exe *.obj
	REM ndisasm -o 0x7c00 mbr.bin
	..\\BUILD\\mkimg -oboot-86.img -n2880 mbr.bin=1 head.bin=2 loader.exe=16
	del *.bin *.obj *.exe

