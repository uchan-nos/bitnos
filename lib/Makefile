include ../Makefile.inc

OBJS   = bnosstartup.o

.PHONY: all, clean, src_only
all:
	$(MAKE) libbnoslibc.a
	$(MAKE) libgolibc.a

clean:
	-$(DEL) *.o
	$(MAKE) -C golibc clean

src_only:
	$(MAKE) clean
	-$(DEL) libbnoslibc.a
	-$(DEL) libgolibc.a
	$(MAKE) -C golibc src_only

libbnoslibc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libgolibc.a:
	$(MAKE) -C golibc libgolibc.a
	$(COPY) golibc/libgolibc.a ./
