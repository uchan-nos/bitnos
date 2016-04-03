/*
 * scancode.h
 *
 *  Created on: 2009/09/16
 *      Author: uchan
 */

#ifndef SCANCODE_H_
#define SCANCODE_H_

#include <bitnos/uncopyable.h>
#include <stdint.h>

class ScanCode : private Uncopyable
{
    private:
            // 修飾されている場合のキーコード変換表
            static const uint8_t tableMod[128];
    public:
           struct Keys
           {
               static const int KeyScrollLock = 0x80;
                   static const int KeyNumLock = 0x81;
                   static const int KeyCapsLock = 0x82;
                   static const int KeyEsc = 0x83;
                   static const int KeyPrintScreen = 0x84;
                   static const int KeyPause = 0x85;
                   static const int KeyLeftShift = 0x86;
                   static const int KeyLeftCtrl = 0x87;
                   static const int KeyLeftAlt = 0x88;
                   static const int KeyRightShift = 0x89;
                   static const int KeyRightCtrl = 0x8a;
                   static const int KeyRightAlt = 0x8b;
                   static const int KeyArrowUp = 0x8c;
                   static const int KeyArrowDown = 0x8d;
                   static const int KeyArrowLeft = 0x8e;
                   static const int KeyArrowRight = 0x8f;
                   static const int KeyInsert = 0x90;
                   static const int KeyDelete = 0x91;
                   static const int KeyHome = 0x92;
                   static const int KeyEnd = 0x93;
                   static const int KeyPageUp = 0x94;
                   static const int KeyPageDown = 0x95;
                   static const int KeyHankakuZenkaku = 0x96;
                   static const int KeyKatakanaHiragana = 0x97;
                   static const int KeyBSlashUnder = 0x98;
                   static const int KeyHenkan = 0x99;
                   static const int KeyMuhenkan = 0x9a;
                   static const int Key = 0x9;
                   static const int KeyF1 = 0xa0;
                   static const int KeyF2 = 0xa1;
                   static const int KeyF3 = 0xa2;
                   static const int KeyF4 = 0xa3;
                   static const int KeyF5 = 0xa4;
                   static const int KeyF6 = 0xa5;
                   static const int KeyF7 = 0xa6;
                   static const int KeyF8 = 0xa7;
                   static const int KeyF9 = 0xa8;
                   static const int KeyF10 = 0xa9;
                   static const int KeyF11 = 0xaa;
                   static const int KeyF12 = 0xab;
                   static const int KeyACPIPower = 0xb0;
                   static const int KeyACPISleep = 0xb1;
                   static const int KeyACPIWake = 0xb2;
                   static const int KeyLeftWin = 0xc0;
                   static const int KeyRightWin = 0xc1;
                   static const int KeyApps = 0xc2;
                   static const int KeyMediaNextTrack = 0xc3;
                   static const int KeyMediaPreviousTrack = 0xc4;
                   static const int KeyMediaStop = 0xc5;
                   static const int KeyMediaPlay = 0xc6;
                   static const int KeyMediaVolumeUp = 0xc7;
                   static const int KeyMediaVolumeDown = 0xc8;
                   static const int KeyMediaSelect = 0xc9;
                   static const int KeyEMail = 0xca;
                   static const int KeyCalculator = 0xcb;
                   static const int KeyMyComputer = 0xcc;
                   static const int KeyWWWSearch = 0xcd;
                   static const int KeyWWWHome = 0xce;
                   static const int KeyWWWBack = 0xcf;
                   static const int KeyWWWForward = 0xd0;
                   static const int KeyWWWStop = 0xd1;
                   static const int KeyWWWRefresh = 0xd2;
                   static const int KeyWWWFavorites = 0xd3;

                   static const int FlagKeypad = 0x0100;
                   static const int FlagBreak = 0x0200;
           };

               struct Code {
                   int code;
               };

               ScanCode()
               {}

               virtual ~ScanCode()
               {}

               /*
                * 独自キーコードをASCIIコードに直す
                * mod : 修飾モードで変換するかどうか
                */
               int ToAscii(Code code, bool mod);

               /*
                * 生のスキャンコードを独自キーコードに変換する
                */
               virtual Code Convert(uint8_t* scancode) = 0;
};

#endif /* SCANCODE_H_ */
