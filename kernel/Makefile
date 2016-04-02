include ../Makefile.inc
include MakefileObjs

# $(OBJSCXX:.o=.d) => foo.d bar/baz.d  if OBJSCXX = "foo.o bar/baz.o"
# $(dir foo.o bar/baz.o) => ./ bar/
# $(notdir foo.d bar/baz.d) => foo.d baz.d
# $(addprefix .,foo.d baz.d) => .foo.d .baz.d
# $(join ./ bar/,.foo.d .baz.d) => ./.foo.d bar/.baz.d
DEPENDS = $(join $(dir $(OBJSCXX)),$(addprefix .,$(notdir $(OBJSCXX:.o=.d))))

.PHONY: all, clean, src_only
all:
	$(MAKE) bitnos.sys

clean:
	$(DEL) $(DEPENDS)
	$(DEL) *.o gui/*.o
	$(DEL) bitnos.hrb hankaku.bin funcs.lst

src_only:
	$(MAKE) clean
	$(DEL) bitnos.sys

bitnos.sys: bitnos.hrb
	$(MAKE) -C ../boot
	$(JOINSYS) bitnos.sys

bitnos.hrb: $(OBJS)
	$(MAKE) -C ../lib
	$(LD) -static -T bitnos.ld -o bitnos.hrb \
		$(OBJS) -L../lib -lgolibc -lbnoslibc

hankaku.o: hankaku.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 hankaku.bin hankaku.o

hankaku.bin: hankaku.txt
	$(MAKEFONT) hankaku.txt hankaku.bin

depend: $(DEPENDS)

-include $(DEPENDS)
