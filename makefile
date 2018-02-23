#boot and make a disk image

#entry point
#same with 'PhyAddrOfKernelEntryPoint' in header_setup.inc
ENTRYPOINT = 0x10400

DISK_IMG:=a.img
FLOPPY:=/mnt/floppy


ASM = nasm
LD = ld
CC = gcc

ASMBFLAGS = -I boot/include/ 
ASMKFLAGS = -I kernel/include/ -f elf
CFLAGS = -I include/ -c -fno-builtin -m32 -fno-stack-protector -minline-all-stringops
LDFLAGS = -s -Ttext $(ENTRYPOINT) -melf_i386




BOOT 	= boot/boot.bin boot/setup.bin
KERNEL 	= kernel.bin
#BLANK 	= lib/blank.bin

OBJS = kernel/kernel.o lib/slib.o kernel/start.o kernel/main.o kernel/protect.o\
		kernel/i8259.o kernel/global.o lib/lib.o kernel/proc.o kernel/syscall.o\
		kernel/keyboard.o kernel/tty.o lib/io.o

.PHONY:everything clean all img realclean

#
#everything:$(BOOT_BIN) $(LOADER_BIN)
#	dd if=$(BOOT_BIN) of=$(DISK_IMG) bs=512 count=1 conv=notrunc
#	sudo mount -o loop $(DISK_IMG) $(FLOPPY)
#	sudo cp $(LOADER_BIN) $(FLOPPY) -v
#	sudo umount $(FLOPPY)


everything:$(BOOT) $(KERNEL) #$(BLANK)

boot/boot.bin:boot/boot.asm 
	$(ASM) $(ASMBFLAGS) -o $@ $<
boot/setup.bin:boot/setup.asm boot/include/header_setup.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<
kernel/kernel.o:kernel/kernel.asm kernel/include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<
kernel/syscall.o: kernel/syscall.asm kernel/include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<


lib/slib.o:lib/slib.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<
lib/lib.o:lib/lib.c include/const.h include/type.h  include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
lib/io.o:lib/io.c include/type.h include/const.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/start.o: kernel/start.c include/const.h include/type.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/main.o: kernel/main.c include/type.h include/const.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/i8259.o:kernel/i8259.c include/type.h include/proto.h include/const.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/protect.o: kernel/protect.c include/type.h include/const.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/global.o:kernel/global.c include/const.h include/global.h include/type.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/proc.o: kernel/proc.c include/type.h include/const.h include/global.h  include/proto.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/keyboard.o: kernel/keyboard.c include/const.h include/proto.h include/type.h
	$(CC) $(CFLAGS) -o $@ $<
kernel/tty.o:kernel/tty.c include/type.h include/const.h include/const.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

#####MOD HERE
#lib/blank.bin:lib/blank.asm
#	$(ASM) -o $@ $<

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

img:
	-rm ./a.img
	cp ../blank_img/a.img ./
	dd if=boot/boot.bin of=$(DISK_IMG) bs=512 count=1 conv=notrunc; \
	Seek=512; \
	SzOfSt=$$(du -sh -b boot/setup.bin | awk '{print $$1}'); \
	dd if=boot/setup.bin of=$(DISK_IMG) bs=1 count=$$SzOfSt  seek=$$Seek conv=notrunc; \
	Seek=$$[$$Seek+$$SzOfSt]; \
	SzOfKnl=$$(du -sh -b kernel.bin | awk '{print $$1}'); \
	dd if=kernel.bin of=$(DISK_IMG) bs=1 count=$$SzOfKnl  seek=$$Seek conv=notrunc; \
#MOD	HERE
#	Seek=$$[$$Seek+$$SzOfKnl]; \
	SzOfBlk=$$(du -sh -b lib/blank.bin | awk '{print $$1}'); \
	dd if=lib/blank.bin of=$(DISK_IMG) bs=1 count=$$SzOfBlk  seek=$$Seek conv=notrunc
	-rm ../run/a.img
	-cp ./a.img ../run




clean: 
	-find . -name '*.o' | xargs rm -rf
	-find . -name '*.bin' | xargs rm -rf
realclean:
	-find . -name '*.o' | xargs rm -rf
	-find . -name '*.bin' | xargs rm -rf
	-find . -name '*.img' | xargs rm -rf

all:everything img clean
