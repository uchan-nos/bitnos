/*
 * Window.h
 *
 *  Created on: 2009/09/09
 *      Author: uchan
 */

#ifndef GUI_WINDOW_H_
#define GUI_WINDOW_H_

class Sheet;

namespace GUI
{

class Control;

/*
 * ウィンドウ生成のための基底クラス
 *
 * 基本的に継承して使う
 */
class Window
{
protected:
  Control** controls;
  int numControl;
  Sheet* sht;

public:
  Window(Control** controlBuffer, Sheet* sht);
  virtual ~Window();

  void Render();

  void Render(Point pos0, Point pos1);
};

}

#endif /* GUI_WINDOW_H_ */
