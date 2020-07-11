dd if=mbr.bin of=boot.img bs=512 count=1
dd if=head.bin of=boot.img seek=1 bs=512 count=1
dd if=loader.exe of=boot.img seek=2 bs=512 count=8
dd if=/dev/zero of=boot.img seek=10 bs=512 count=2870