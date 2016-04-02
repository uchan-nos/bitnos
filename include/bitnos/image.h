/*
 * image.h
 *
 *  Created on: 2009/07/06
 *      Author: uchan
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <bitnos/point.h>
#include <bitnos/IDraw.h>

/*
 * 色変換関数
 */
uint16_t ConvRGB16(uint8_t r, uint8_t g, uint8_t b);
uint16_t ConvRGB16(uint32_t col);

/*
 * イメージデータ操作クラス
 */
class Image : public IDraw
{
public:
  uint16_t* buf; // イメージのデータを格納するバッファへのポインタ
  int width; // イメージの幅
  int height; // イメージの高さ

  bool allocated;

public:
  /*
   * コンストラクタ
   * buf : イメージデータのバッファ
   * width : イメージの幅
   * height : イメージの高さ
   */
  Image(uint16_t* buf, int width, int height);

  /*
   * コンストラクタ
   * イメージデータのバッファを自動生成する
   * width : イメージの幅
   * height : イメージの高さ
   */
  Image(int width, int height);

  /*
   * コピーコンストラクタ
   * イメージデータのバッファを自動生成する
   */
  Image(const Image& rhs);

  /*
   * デストラクタ
   */
  virtual ~Image();

  int GetWidth() const;
  int GetHeight() const;

  void DrawPoint(uint16_t col, int xPos, int yPos)
  {
    if (0 <= xPos && xPos < width && 0 <= yPos && yPos < height) {
      this->buf[yPos * this->width + xPos] = col;
    }
  }

  void DrawRectangle(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1);
  void DrawRectangleFill(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1);

  void DrawLine(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1);

  void DrawChar(uint16_t col, char ch, int xPos, int yPos, BitmapFont* font);

  void DrawString(uint16_t col, const char* str, int xPos, int yPos, BitmapFont* font);

  uint16_t GetPoint(int xPos, int yPos) const
  {
    return this->buf[yPos * this->width + xPos];
  }

  uint16_t GetPoint(Point pos) const
  {
    return buf[pos.Y * width + pos.X];
  }

  // IDrawの実装
  virtual void DrawPoint(uint16_t col, Point pos);
  /*
  {
    DrawPoint(col, pos.X, pos.Y);
  }
  */

  virtual void DrawRectangle(uint16_t col, Point pos0, Point pos1);
  /*
  {
    DrawRectangle(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
  }
  */
  virtual void DrawRectangleFill(uint16_t col, Point pos0, Point pos1);
  /*
  {
    DrawRectangleFill(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
  }
  */

  virtual void DrawLine(uint16_t col, Point pos0, Point pos1);
  /*
  {
    DrawLine(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
  }
  */

  virtual void DrawChar(uint16_t col, char ch, Point pos, BitmapFont* font);

  virtual void DrawString(uint16_t col, const char* str, Point pos, BitmapFont* font);
  /*
  {
    DrawString(col, str, pos.X, pos.Y, font);
  }
  */

};

#endif /* IMAGE_H_ */
