/*
 * TextBox.h
 *
 *  Created on: 2009/09/23
 *      Author: uchan
 */

#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include <bitnos/gui/Control.h>
#include <bitnos/gui/Label.h>
#include <bitnos/image.h>
#include <bitnos/font.h>

namespace GUI
{

class TextBox : public Control
{
protected:
  Label label1;
  Point pos;
  uint16_t bgcolor;
public:
  /*
   * pos : コントロールの位置
   * buf : 文字列のバッファ
   * len : バッファのバイト数
   * font : 文字のフォント
   * bgcolor : 背景色
   */
  TextBox(Point pos, char* buf, int len, BitmapFont* font, uint16_t bgcolor);

  virtual ~TextBox();

  virtual void Render(Image* img, Point pos0, Point pos1);

  virtual int GetWidth() const
  {
    return label1.GetWidth() + 4;
  }

  virtual int GetHeight() const
  {
    return label1.GetHeight() + 4;
  }

  virtual Point GetPosition() const
  {
    return pos;
  }

  virtual void ShowCursor()
  {
    label1.ShowCursor();
  }

  virtual void ClearCursor()
  {
    label1.ClearCursor();
  }

};

}

#endif /* TEXTBOX_H_ */
