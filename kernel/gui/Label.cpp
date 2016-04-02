/*
 * Label.cpp
 *
 *  Created on: 2009/09/14
 *      Author: uchan
 */

#include <bitnos/gui/Label.h>
#include <string.h>

namespace GUI
{

    Label::Label(Point pos, char *buf, int len, BitmapFont* font) :
        pos(pos), font(font), value(buf), length(len), cursor(false)
    {
    }



    Label::~Label()
    {
    }



    void Label::Render(Image *img, Point pos0, Point pos1)
    {
        if ((pos0.X < pos.X + GetWidth() && pos0.Y < pos.Y + GetHeight()) &&
                (pos1.X >= pos.X && pos1.Y >= pos.Y)) {
            int len = strlen(value);
            img->DrawString(0, value, pos, font);
            if (cursor) {
                img->DrawRectangleFill(ConvRGB16(0x00, 0x00, 0x00),
                        pos + Point(len * font->GetWidth(), 0),
                        pos + Point((len + 1) * font->GetWidth() - 1, font->GetHeight() - 1));
            }
        }
    }

    void Label::SetValue(const char* str)
    {
        strncpy(value, str, length);
    }

    char* Label::GetValue(void)
    {
        return value;
    }

    void Label::ShowCursor()
    {
        cursor = true;
    }

    void Label::ClearCursor()
    {
        cursor = false;
    }

}
