/*
 * kbc.cpp
 *
 *  Created on: 2009/08/01
 *      Author: uchan
 */

#include <bitnos/kbc.h>
#include <bitnos/funcs.h>


void KBC::WaitBusy()
{
    while (
            (io_in8(Port::Status) & 0x02) != 0
          );
    /*
       Status st;
       while (
       st.Byte = io_in8(Port::Status), st.Bit.Busy
       );*/
    /*
       while (
       ((Status)(io_in8(Port::Status))).Bit.Busy
       );
       */
}

void KBC::SetMode(uint8_t data)
{
    WaitBusy();

    // コマンド書き込みモード
    io_out8(Port::Command, 0x60);

    WaitBusy();

    // モードレジスタへ書き込み
    // システムフラグはとりあえず1にしておく
    io_out8(Port::Data, data | 0x04);
}

void KBC::SendToMouse(uint8_t data)
{
    WaitBusy();

    // マウスへ送信するモード
    io_out8(Port::Command, 0xd4);

    WaitBusy();

    // マウスへ送信
    io_out8(Port::Data, data);
}

uint8_t KBC::ReceiveData()
{
    return (uint8_t)io_in8(Port::Data);
}

void KBC::SendCommand(uint8_t data)
{
    WaitBusy();

    io_out8(Port::Data, data);
}
