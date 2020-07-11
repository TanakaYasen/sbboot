# Overview
	sbboot , explore the bootloader on X86-64 CPU machine.	sb means simple and basic.

## intensions
	To study how to make boot-image, how to read HD, operate video memory(output "Hello word"), in&output ports, DMA, and how to switch to protect mode or long mode, paging, and so on.
	Involving setting-up Interrupt Vector, handling exceptions and interrupts.
	We also wanna understand how CPU work and how to control hardwares under the hood.
	
## Env
	Bochs 2.6+(VMware if ok, but bochs provide rich debug methods), toolchain-VS, NASM, and some code editors.
	
## how to build
	Open a cmd window, cd project's root, set toolchain PATH, and run nmake. Above steps burn a "Boot.img" to a virtual Disk&Floppy;
 
 ## how to test
	New a bochs VirtualMachine, set Disk&Floppy image to our image. and run with bochsdbg.exe
