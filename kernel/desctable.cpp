/*
 * desctable.cpp
 *
 *  Created on: 2009/07/12
 *      Author: uchan
 */

#include <bitnos/desctable.h>
#include <bitnos/funcs.h>

void SegmentDescriptor::Init(uint32_t limit, uint32_t base, uint8_t segtype, uint8_t seg32type, uint8_t dpl)
{
    if (limit > 0xfffff) {
        seg32type |= 0x8; // G bit
        limit >>= 12; // limit /= 4096;
    }
    this->limitL = (limit & 0x0000ffff);
    this->limitH = ((limit >> 16) & 0x0f) | ((seg32type << 4) & 0xf0);
    this->baseL =  (base & 0x0000ffff);
    this->baseM = ((base >> 16) & 0xff);
    this->baseH = ((base >> 24) & 0xff);
    this->type = segtype | (dpl << 5);
}

void GateDescriptor::Init(uint32_t offset, uint16_t selector, uint8_t count, uint8_t type, uint8_t dpl)
{
    this->offsetL = (offset & 0x0000ffff);
    this->offsetH = ((offset >> 16) & 0x0000ffff);
    this->selector = selector;
    this->count = count;
    this->type = type | (dpl << 5);
}

void GDT::Init()
{
    SegmentDescriptor* gdt = (SegmentDescriptor*)SEGMDESCTBL_ADDR;

    for (int i = 0; i < 8192; i++) {
        gdt[i].Init(0, 0, 0, 0, 0);
    }

    // データセグメント
    gdt[1].Init(0xffffffff, 0x00000000, TYPE_DATA_RW, TYPE_32, 0);

    // コードセグメント
    gdt[2].Init(1024 * 1024 - 1, 0x01000000, TYPE_CODE_RX, TYPE_32, 0);

    load_gdtr(0xffff, SEGMDESCTBL_ADDR);
}

void IDT::Init()
{
    GateDescriptor* idt = (GateDescriptor*)GATEDESCTBL_ADDR;

    for (int i = 0; i < 256; i++) {
        idt[i].Init(0, 0, 0, 0, 0);
    }

    load_idtr(0x7ff, GATEDESCTBL_ADDR);

    idt[0x01].Init((uint32_t)asm_inthandler01, 2 * 8, 0, TYPE_INTGATE, 0);
    idt[0x0d].Init((uint32_t)asm_inthandler0d, 2 * 8, 0, TYPE_INTGATE, 0);
    idt[0x20].Init((uint32_t)asm_inthandler20, 2 * 8, 0, TYPE_INTGATE, 0);
    idt[0x21].Init((uint32_t)asm_inthandler21, 2 * 8, 0, TYPE_INTGATE, 0);
    idt[0x2c].Init((uint32_t)asm_inthandler2c, 2 * 8, 0, TYPE_INTGATE, 0);
}

