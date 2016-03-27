.PHONY: all

all: floppy.img

floppy.img: kernel/loader.bin
	cp $< $@

kernel/loader.bin: kernel/loader.asm
	nasm -f bin -o $@ $<
