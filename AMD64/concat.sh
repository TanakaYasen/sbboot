dd if=mbr.bin of=boot-64.img bs=512 count=1
dd if=head.bin of=boot-64.img seek=1 bs=512 count=2
dd if=loader.exe of=boot-64.img seek=3 bs=512 count=17
dd if=/dev/zero of=boot-64.img seek=20 bs=512 count=2860