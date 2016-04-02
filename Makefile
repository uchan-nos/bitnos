include ./Makefile.inc

.PHONY: all, run, clean, src_only, install
all:
	$(MAKE) -C lib
	$(MAKE) -C boot
	$(MAKE) -C kernel
	$(MAKE) bitnos.fd.img

run:
	$(MAKE)
	$(QEMU) -L . -m 32 -localtime -vga std \
	-drive file=bitnos.fd.img,if=floppy,format=raw

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean

src_only:
	$(MAKE) -C boot src_only
	$(MAKE) -C kernel src_only
	$(MAKE) -C lib src_only
	$(DEL) bitnos.fd.img

install:
	$(IMGTOL) w a: bitnos.fd.img

bitnos.fd.img:
	$(EDIMG) imgin:$(TOLSET_PATH)/z_tools/fdimg0at.tek \
		wbinimg src:boot/ipl.bin len:512 from:0 to:0 \
		copy from:kernel/bitnos.sys to:@: \
		imgout:bitnos.fd.img
