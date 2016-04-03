/*
 * Label.h
 *
 *  Created on: 2009/09/14
 *      Author: uchan
 */

#ifndef LABEL_H_
#define LABEL_H_

#include <bitnos/gui/Control.h>
#include <bitnos/image.h>
#include <bitnos/font.h>

namespace GUI
{

class Label : public Control
{
protected:
  Point pos;
  BitmapFont* font;
  char* value;
  int length;
  bool cursor;
public:
  Label(Point pos, char* buf, int len, BitmapFont* font);
  virtual ~Label();

  virtual void Render(Image* img, Point pos0, Point pos1);

  virtual int GetWidth() const
  {
    return font->GetWidth() * length;
  }

  virtual int GetHeight() const
  {
    return font->GetHeight();
  }

  virtual Point GetPosition() const
  {
    return pos;
  }

  virtual void SetValue(const char* str);

  virtual char* GetValue(void);

  virtual void ShowCursor();

  virtual void ClearCursor();
};

}



#endif /* LABEL_H_ */
