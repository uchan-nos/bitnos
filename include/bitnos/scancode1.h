/*
 * scancode1.h
 *
 *  Created on: 2009/09/05
 *      Author: uchan
 */

#ifndef SCANCODE1_H_
#define SCANCODE1_H_

#include <stdint.h>
#include <bitnos/scancode.h>

class ScanCode01 : public ScanCode
{
private:
  // スキャンコードから独自コードに変換するテーブル
  static const uint8_t table[128]; // ノーマルスキャンコード
  static const uint8_t tableE0[128]; // E0拡張キーコード

public:
  uint8_t ToAscii(Code code, bool mod);

  Code Convert(uint8_t* scancode);
};

#endif /* SCANCODE1_H_ */
