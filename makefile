# set PATH=D:\Program Files\nasm-2.15.01;%PATH%;D:\repos\WRK1.2\tools\x86;
# also MSYS env is involved.

BASH = "D:\Program Files\SmartGit\git\git-bash.exe"
CFLAGS = -nologo -Zl -Zp8 -Gy -cbstring -W3 -WX -GR- -GF -GS -Gm- -Gz -Z7 -Oxs -Oy-
LFLAGS =  -nodefaultlib -machine:x86  -driver -subsystem:native,5.02 -WX -NODEFAULTLIB -machine:x86 -release -version:5.2 -osversion:5.2

all:
	nasm mini-loader.asm -o mbr.bin
	nasm mini-head.asm -o head.bin
	
	cl.exe $(CFLAGS) -c loader.c -out:loader.obj
	cl.exe $(CFLAGS) -c idt.c -out:idt.obj
	cl.exe $(CFLAGS) -c gdt.c -out:gdt.obj
	cl.exe $(CFLAGS) -c ReadHD.c -out:ReadHD.obj
	cl.exe $(CFLAGS) -c printk.c -out:printk.obj
	nasm.exe inout.asm -fwin32 -o inout.obj

	link.exe $(LFLAGS) -entry:loader_main -base:0x100000 /out:loader.exe *.obj
	ndisasm -o 0x7c00 mbr.bin
	$(BASH) concat.sh
	del *.bin *.obj 

