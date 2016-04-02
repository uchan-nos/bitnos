/*
 * TextBox.cpp
 *
 *  Created on: 2009/09/24
 *      Author: uchan
 */

#include <bitnos/gui/TextBox.h>
#include <string.h>

namespace GUI
{

    TextBox::TextBox(Point pos, char* buf, int len, BitmapFont* font, uint16_t bgcolor) :
        label1(pos + Point(2, 2), buf, len, font),
        pos(pos), bgcolor(bgcolor)
    {
    }

    TextBox::~TextBox()
    {
    }

    void TextBox::Render(Image* img, Point pos0, Point pos1)
    {
        /*
           img->DrawRectangle(
           ConvRGB16(0x00, 0x00, 0x00), pos, pos + Point(GetWidth() - 1, GetHeight() - 1));
           */
        if (pos0.X < pos.X + 2 ||
                pos1.X > pos.X + GetWidth() - 3 ||
                pos0.Y < pos.Y + 2 ||
                pos1.Y > pos.Y + GetHeight() - 3) {
            img->DrawLine(
                    ConvRGB16(0x80, 0x80, 0x80),
                    pos + Point(0, 0), pos + Point(GetWidth() - 1, 0));
            img->DrawLine(
                    ConvRGB16(0x80, 0x80, 0x80),
                    pos + Point(0, 1), pos + Point(0, GetHeight() - 2));
            img->DrawLine(
                    ConvRGB16(0xff, 0xff, 0xff),
                    pos + Point(0, GetHeight() - 1), pos + Point(GetWidth() - 1, GetHeight() - 1));
            img->DrawLine(
                    ConvRGB16(0xff, 0xff, 0xff),
                    pos + Point(GetWidth() - 1, 1), pos + Point(GetWidth() - 1, GetHeight() - 1));
            img->DrawRectangleFill(
                    bgcolor, pos + Point(1, 1), pos + Point(GetWidth() - 2 ,GetHeight() - 2));
        } else {
            img->DrawRectangleFill(
                    bgcolor, pos0, pos1);
        }
        label1.Render(img, pos0, pos1);
    }

}
