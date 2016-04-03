/*
 * funcs.h
 *
 *  Created on: 2009/07/05
 *      Author: uchan
 */

#ifndef FUNCS_H_
#define FUNCS_H_

#include <stdint.h>

extern "C"
{
    void io_hlt(void);
    void io_cli(void);
    void io_sti(void);
    void io_stihlt(void);
    uint32_t io_load_eflags(void);
    void io_store_eflags(uint32_t eflags);
    uint32_t load_cr0(void);
    void store_cr0(uint32_t cr0);
    void load_gdtr(uint32_t limit, uint32_t addr);
    void load_idtr(uint32_t limit, uint32_t addr);
    void asm_inthandler01();
    void asm_inthandler0d();
    void asm_inthandler20();
    void asm_inthandler21();
    void asm_inthandler2c();
    uint32_t io_in8(uint32_t port);
    uint32_t io_in16(uint32_t port);
    uint32_t io_in32(uint32_t port);
    void io_out8(uint32_t port, uint32_t data);
    void io_out16(uint32_t port, uint32_t data);
    void io_out32(uint32_t port, uint32_t data);
    void load_tr(uint32_t tr);
    void farjmp(uint32_t eip, uint32_t cs);
    uint32_t load_dr0();
    uint32_t load_dr1();
    uint32_t load_dr2();
    uint32_t load_dr3();
    uint32_t load_dr6();
    uint32_t load_dr7();
    void store_dr0(uint32_t dr0);
    void store_dr1(uint32_t dr1);
    void store_dr2(uint32_t dr2);
    void store_dr3(uint32_t dr3);
    void store_dr6(uint32_t dr6);
    void store_dr7(uint32_t dr7);
    uint32_t load_cs();
    uint32_t load_eip();
}

#endif /* FUNCS_H_ */
