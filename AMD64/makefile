# set PATH=D:\Program Files\nasm-2.15.01;%PATH%;D:\repos\WRK1.2\tools\AMD64;
# also MSYS env is involved.

ASFLAGS = -I../common
CFLAGS = -nologo -Zl -Zp8 -Gy -cbstring -W3 -WX -GR- -GF -GS- -Gm- -Gz -Z7 -Oxs -Oy-
LFLAGS =  -nodefaultlib -machine:x64  -driver -subsystem:native,5.02 -WX -release -version:5.2 -osversion:5.2 -IGNORE:4281


all:
	nasm $(ASFLAGS) ../common/mini-mbr.asm -o mbr.bin
	nasm mini-head-x64.asm -o head.bin

	cl.exe $(CFLAGS) -c loader.c
	cl.exe $(CFLAGS) -c apic.c
	cl.exe $(CFLAGS) -c mm.c
	cl.exe $(CFLAGS) -c ../common/printk.c
	nasm.exe -fwin64 util.asm

	link.exe $(LFLAGS) -entry:loader_main -base:0x100000 -map:loader.map /out:loader.exe *.obj
	REM ndisasm -o 0x7c00 mbr.bin
	..\\BUILD\\mkimg -oboot-64.img -n2880 mbr.bin head.bin loader.exe
	del *.bin *.obj *.exe

