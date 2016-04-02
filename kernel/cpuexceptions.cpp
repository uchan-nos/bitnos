/*
 * cpuexceptions.cpp
 *
 *  Created on: 2009/08/17
 *      Author: uchan
 */

#include <bitnos/cpuexceptions.h>
#include <bitnos/debug.h>
#include <bitnos/funcs.h>
#include <bitnos/mtask.h>

uint32_t* inthandler01(uint32_t* esp)
{
    Debug::WriteLine("INT 01 (debug)");
    Debug::WriteLine("cs:eip=%04x:%08x,dr6=%08x", esp[11], esp[10], load_dr6());
    for (;;) io_hlt();
    return 0;
}

uint32_t* inthandler0d(uint32_t* esp)
{
    Debug::WriteLine("INT 0D");
    Debug::WriteLine("cs:eip=%04x:%08x",esp[12], esp[11]);
    for (;;) io_hlt();
    return 0;
}

