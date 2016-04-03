/*
 * segment.h
 *
 *  Created on: 2009/07/12
 *      Author: uchan
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>

#define SEGMDESCTBL_ADDR (0x00010000)
#define GATEDESCTBL_ADDR (0x00020000)

#define TYPE_32      (0x4)
#define TYPE_DATA_RW (0x92)
#define TYPE_CODE_RX (0x9a)
#define TYPE_INTGATE (0x8e)
#define TYPE_TSS32   (0x89)
#define TYPE_LDT     (0x82)


struct SegmentDescriptor
{
    uint16_t limitL;
    uint16_t baseL;
    uint8_t baseM;
    uint8_t type;
    uint8_t limitH;
    uint8_t baseH;

    void Init(uint32_t limit, uint32_t base, uint8_t segtype, uint8_t seg32type, uint8_t dpl);
};

struct GateDescriptor
{
    uint16_t offsetL;
    uint16_t selector;
    uint8_t count;
    uint8_t type;
    uint16_t offsetH;

    void Init(uint32_t offset, uint16_t selector, uint8_t count, uint8_t type, uint8_t dpl);
};

class GDT : private Uncopyable
{
    public:
        static void Init();
};

class IDT : private Uncopyable
{
    public:
        static void Init();
};

#endif /* SEGMENT_H_ */
