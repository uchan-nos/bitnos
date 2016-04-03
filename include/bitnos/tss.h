/*
 * tss.h
 *
 *  Created on: 2009/08/12
 *      Author: uchan
 */

#ifndef TSS_H_
#define TSS_H_

#include <stdint.h>

struct TSS32
{
    uint32_t backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldtr, iomap;
};


#endif /* TSS_H_ */
