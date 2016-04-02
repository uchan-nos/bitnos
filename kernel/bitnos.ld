OUTPUT_FORMAT("binary")
ENTRY(BNosMain)

HEAP_SIZE = 0;
MMAREA_SIZE = 0;

__ctors = ADDR(.ctors);
__ctors_count = SIZEOF(.ctors) / 4;

SECTIONS
{
    . = 0;
    .head : AT(0)
    {
        CODESEG_SIZE = "file offset of data" - LOADADDR(.head);
        HEAP_ADDR = ADDR(.bss) + SIZEOF(.bss);
        DATASEG_SIZE = (HEAP_ADDR + HEAP_SIZE + 0xfff) & 0xfffff000;
        MMAREA_SIZE = (MMAREA_SIZE + 0xfff) & 0xfffff000;
        LONG(0xe9000000)                          /*  0: jmp */
        LONG(BNosMain - 8)                      /*  4: start up procedure */
        LONG(0x534f4e42)                          /*  8: signature "BNOS" */
        LONG(CODESEG_SIZE)                        /* 12: size of code segment */
        LONG(DATASEG_SIZE)                        /* 16: size of data segment */
        LONG(MMAREA_SIZE)                         /* 20: size of mm area */
        LONG(ADDR(.ctors))                        /* 24: initial stack pointer & where copy data to */
        LONG(ADDR(.bss) - ADDR(.ctors))           /* 28: size of data to copy */
        LONG("file offset of data")               /* 32: file offset of data to copy */
        LONG(HEAP_ADDR)                           /* 36: addr of heap area */
    }

    .text : { *(.text*) }
    . = ALIGN(8);
    "file offset of data" = .;

    . = 0x310000;
    .ctors : AT("file offset of data") { *(.ctors) }

    .data : AT(ADDR(.data) - ADDR(.ctors) + "file offset of data") { *(.data*) }

    .rodata : AT(ADDR(.rodata) - ADDR(.ctors) + "file offset of data") { *(.rodata*) }

    .bss : AT (ADDR(.bss) - ADDR(.ctors) + "file offset of data") { *(.bss*) }

    /DISCARD/ : { *(.comment) *(.eh_frame) }

}
