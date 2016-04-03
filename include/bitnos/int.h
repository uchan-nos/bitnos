/*
 * int.h
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#ifndef INT_H_
#define INT_H_

#include <stdint.h>

class PIC
{
    private:
            static const uint32_t PIC0_ICW1 = 0x0020;
                static const uint32_t PIC0_OCW2 = 0x0020;
                static const uint32_t PIC0_IMR  = 0x0021;
                static const uint32_t PIC0_ICW2 = 0x0021;
                static const uint32_t PIC0_ICW3 = 0x0021;
                static const uint32_t PIC0_ICW4 = 0x0021;
                static const uint32_t PIC1_ICW1 = 0x00a0;
                static const uint32_t PIC1_OCW2 = 0x00a0;
                static const uint32_t PIC1_IMR  = 0x00a1;
                static const uint32_t PIC1_ICW2 = 0x00a1;
                static const uint32_t PIC1_ICW3 = 0x00a1;
                static const uint32_t PIC1_ICW4 = 0x00a1;
    public:
           static const uint32_t IRQ0  = (1 << 0 );
               static const uint32_t IRQ1  = (1 << 1 );
               static const uint32_t IRQ2  = (1 << 2 );
               static const uint32_t IRQ3  = (1 << 3 );
               static const uint32_t IRQ4  = (1 << 4 );
               static const uint32_t IRQ5  = (1 << 5 );
               static const uint32_t IRQ6  = (1 << 6 );
               static const uint32_t IRQ7  = (1 << 7 );
               static const uint32_t IRQ8  = (1 << 8 );
               static const uint32_t IRQ9  = (1 << 9 );
               static const uint32_t IRQ10 = (1 << 10);
               static const uint32_t IRQ11 = (1 << 11);
               static const uint32_t IRQ12 = (1 << 12);
               static const uint32_t IRQ13 = (1 << 13);
               static const uint32_t IRQ14 = (1 << 14);
               static const uint32_t IRQ15 = (1 << 15);

    public:

               static void Init();

               static void SetMask(uint32_t mask);

               static void Notify(uint32_t irqnum);
};

/*
 * 割り込み禁止マクロ
 */
#define DINT \
uint32_t __eflags__ = io_load_eflags(); \
io_cli();

/*
 * eflags復帰マクロ
 */
#define RINT \
io_store_eflags(__eflags__);

#endif /* INT_H_ */
