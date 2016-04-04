CC=gcc
LD=ld
NASM=nasm

.PHONY: all clean

all: floppy.img

clean:
	rm -f floppy.img kernel/*.bin kernel/*.o

floppy.img: kernel/loader.bin kernel/kernel.bin
	@echo writing floppy.img
	@dd of=$@ if=/dev/zero bs=512 count=2880 status=none
	@dd of=$@ if=kernel/loader.bin bs=512 seek=0 conv=notrunc status=none
	@dd of=$@ if=kernel/kernel.bin bs=512 seek=1 conv=notrunc status=none

kernel/loader.bin: kernel/loader.asm
	@echo nasm $@
	@$(NASM) -f bin -o $@ $<

kernel/kernel.bin: kernel/start.o kernel/main.o kernel/serial.o
	@echo ld $@
	@$(LD) -T kernel/linker.ld -o $@ $^

kernel/%.o: kernel/%.asm
	@echo nasm $@
	@$(NASM) -f elf64 -o $@ $<

kernel/%.o: kernel/%.c kernel/*.h
	@echo cc $@
	@$(CC) -o $@ -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -nostdlib -c $<
