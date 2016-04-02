include ../../Makefile.inc

OBJS   = abs.o atof.o atoi.o errno.o frexp.o ldexp.o \
	memchr.o memcmp.o memmove.o memset.o qsort.o rand.o \
	sprintf.o strcat.o strcmp.o strcpy.o strcspn.o strdup.o \
	strlen.o strncat.o strncmp.o strncpy.o strpbrk.o strrchr.o \
	strspn.o strstr.o strtol.o strtoul.o strtoul0.o vsprintf.o

.PHONY: all, clean, src_only
all:
	$(MAKE) libgolibc.a

clean:
	-$(DEL) *.o
	-$(DEL) *.lst

src_only:
	$(MAKE) clean
	-$(DEL) libgolibc.a

libgolibc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
