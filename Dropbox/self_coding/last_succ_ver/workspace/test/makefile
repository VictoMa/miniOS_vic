.PHONY:everything
everything:
	-rm ../../run/a.img
	#nasm test.asm -o test.bin
	dd if=test.bin of=a.img bs=1 count=512 conv=notrunc
	cp ./a.img ../../run/