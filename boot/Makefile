include ../Makefile.inc

all:
	$(MAKE) boot.bin ipl.bin

%.bin: %.asm
	$(NASM) -f bin -o $@ -l $*.lst $<

clean:
	$(DEL) *.lst

src_only:
	$(MAKE) clean
	$(DEL) *.bin
	
