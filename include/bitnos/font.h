/*
 * font.h
 *
 *  Created on: 2009/07/11
 *      Author: uchan
 */

#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>

class BitmapFont
{
  int width; // 1文字の幅
  int height; // 1文字の高さ
  void* data; // フォントデータの開始アドレス
  int dataSize; // フォントデータの容量(byte)
  int increment; // 1文字の容量(byte)
public:
  void Init(int width, int height, void* data, int dataSize, int increment);

  int GetWidth()
  {
    return this->width;
  }

  int GetHeight()
  {
    return this->height;
  }

  void* GetDataPtr(int c)
  {
    return (uint8_t*)this->data + (c * this->increment);
  }

};

// hankaku.txt
extern char _binary_hankaku_bin_start[];
extern char _binary_hankaku_bin_end[];
extern char _binary_hankaku_bin_size[];

extern BitmapFont* fontHankaku;

#endif /* FONT_H_ */
