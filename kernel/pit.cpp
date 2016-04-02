/*
 * pit.cpp
 *
 *  Created on: 2009/08/11
 *      Author: uchan
 */

#include <bitnos/pit.h>
#include <bitnos/int.h>
#include <bitnos/funcs.h>

void inthandler20(int* esp)
{
    // 割り込み確認をPICに通知
    PIC::Notify(0);

    // 割り込みハンドラ起動
    PIT::IntHandler();
}

void PIT::Init()
{
    /*
     * PITのチャンネル0（IRQ0用）の設定
     *
     * Clock = 1.19318MHz
     * 約10msec周期に設定
     */
    io_out8(Port::CR, CountMode::M2Bin | CountAccessMode::LH | CH::CH0);
    io_out8(Port::Cnt0, 11932 & 0xff);
    io_out8(Port::Cnt0, (11932 >> 8) & 0xff);
}

void PIT::IntHandler()
{
    timman->IntHandler();
}

void PIT::BeetSetCounter(int cnt)
{
    io_out8(PIT::Port::CR, PIT::CH::CH2 | PIT::CountAccessMode::LH | PIT::CountMode::M3Bin);
    io_out8(PIT::Port::Cnt2, cnt & 0x00ff);
    io_out8(PIT::Port::Cnt2, (cnt >> 8) & 0x00ff);
}

void PIT::BeepOn()
{
    uint8_t reg = io_in8(PIT::Port::System);
    reg |= 0x03;
    reg &= 0x0f;
    io_out8(PIT::Port::System, reg);
}

void PIT::BeepOff()
{
    uint8_t reg = io_in8(PIT::Port::System);
    reg &= 0x0d;
    io_out8(PIT::Port::System, reg);
}

