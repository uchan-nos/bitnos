/*
 * Control.h
 *
 *  Created on: 2009/09/09
 *      Author: uchan
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include <bitnos/point.h>

class Image;


namespace GUI
{
class Control
{
protected:
public:
  Control()
  {}

  virtual ~Control()
  {}

  /*
   * 与えられたImageバッファに自分を描画する
   * 指定された範囲（pos0からpos1）に含まれる部分だけ描画すればよい
   * pos0, pos1はウィンドウの左上を基準とする座標
   */
  virtual void Render(Image* img, Point pos0, Point pos1) = 0;

  virtual int GetWidth() const = 0;
  virtual int GetHeight() const = 0;

  virtual Point GetPosition() const = 0;
};
}

#endif /* CONTROL_H_ */
