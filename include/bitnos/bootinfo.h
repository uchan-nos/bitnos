/*
 * bootinfo.h
 *
 *  Created on: 2009/07/05
 *      Author: uchan
 */

#ifndef BOOTINFO_H_
#define BOOTINFO_H_

#include <stdint.h>

/*
 * 起動情報の構造体
 */
struct BootInfo
{
    uint32_t sectors;  // Floppy Diskの読み込みセクタ数
    uint32_t leds;     // キーボードのLED状態
    uint32_t color;    // 画面色数
    uint32_t xsize;    // 画面X方向解像度
    uint32_t ysize;    // 画面Y方向解像度
    uint32_t vramaddr; // VRAMのアドレス
};

#define BOOTINFO_ADDR (0x00000f00)

#endif /* BOOTINFO_H_ */
