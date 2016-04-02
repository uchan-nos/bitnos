/*
 * int.cpp
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#include <bitnos/int.h>
#include <bitnos/funcs.h>

void PIC::Init()
{
    io_out8(PIC::PIC0_IMR, 0xff); // すべての割り込み禁止
    io_out8(PIC::PIC1_IMR, 0xff); // すべての割り込み禁止

    io_out8(PIC::PIC0_ICW1, 0x11); // エッジトリガモード
    io_out8(PIC::PIC0_ICW2, 0x20); // IRQ0-7 : INT20-27
    io_out8(PIC::PIC0_ICW3, 0x04); // PIC1 : IRQ2
    io_out8(PIC::PIC0_ICW4, 0x01); // ノンバッファモード

    io_out8(PIC::PIC1_ICW1, 0x11); // エッジトリガモード
    io_out8(PIC::PIC1_ICW2, 0x28); // IRQ8-15 : INT28-2f
    io_out8(PIC::PIC1_ICW3, 2   ); // PIC1 : IRQ2
    io_out8(PIC::PIC1_ICW4, 0x01); // ノンバッファモード

    io_out8(PIC::PIC0_IMR, 0xfb); // 許可 : PIC1
    io_out8(PIC::PIC1_IMR, 0xff); // 許可 :
}

void PIC::SetMask(uint32_t mask)
{
    mask = ~mask;
    io_out8(PIC::PIC0_IMR, mask & 0xff);
    io_out8(PIC::PIC1_IMR, (mask >> 8) & 0xff);
}

void PIC::Notify(uint32_t irqnum)
{
    if (irqnum < 0x08) {
        io_out8(PIC::PIC0_OCW2, irqnum + 0x60);
    } else if (irqnum < 0x10) {
        io_out8(PIC::PIC1_OCW2, irqnum + 0x60 - 0x08);
        io_out8(PIC::PIC0_OCW2, 0x62);
    }
}
