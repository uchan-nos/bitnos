/*
 * kbc.h
 *
 *  Created on: 2009/08/01
 *      Author: uchan
 */

#ifndef KBC_H_
#define KBC_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>

/*
 * Keyboard Controller
 */
class KBC : private Uncopyable
{
    private:
            struct Port : private Uncopyable
    {
        static const uint16_t Data    = 0x0060;
            static const uint16_t Status  = 0x0064;
            static const uint16_t Command = 0x0064;
    };

                union Status
                {
                    uint8_t Byte;
                        struct {
                            unsigned Parity: 1;
                                unsigned ReceiveError: 1;
                                unsigned SendError: 1;
                                unsigned InputEnable: 1;
                                unsigned SendingDataKind: 1;
                                unsigned SystemFlag: 1;
                                unsigned Busy: 1;
                                unsigned NotEmpty: 1;
                        } Bit;
                };

    public:
           struct Mode : private Uncopyable {
               static const uint8_t UseScancode01       = 0x40;
                   static const uint8_t DisableMouse        = 0x20;
                   static const uint8_t DisableKeyboard     = 0x10;
                   static const uint8_t DisableKeyboardLock = 0x08;
                   static const uint8_t EnableMouseIRQ      = 0x02;
                   static const uint8_t EnableKeyboardIRQ   = 0x01;
           };

               struct Command : private Uncopyable {
                   static const uint8_t SetLEDs        = 0xed;
                       static const uint8_t SetScanCodeSet = 0xf0;
               };

    private:
            /*
             * KBCがデータ送信可能になるのを待つ
             */
            static void WaitBusy();

    public:
           /*
            * モードレジスタに書き込む
            */
           static void SetMode(uint8_t data);

               /*
                * マウスにデータを送る
                */
               static void SendToMouse(uint8_t data);

               /*
                * キーボードデータを読み取る
                */
               static uint8_t ReceiveData();

               /*
                * KBCにコマンドを送る
                */
               static void SendCommand(uint8_t data);

};

#endif /* KBC_H_ */
