/*
 * IDraw.h
 *
 *  Created on: 2009/07/30
 *      Author: uchan
 */

#ifndef IDRAW_H_
#define IDRAW_H_

#include <bitnos/point.h>
#include <bitnos/font.h>

class IDraw
{
    public:
        virtual void DrawPoint(uint16_t col, Point pos) = 0;

        virtual void DrawRectangle(uint16_t col, Point pos0, Point pos1) = 0;
        virtual void DrawRectangleFill(uint16_t col, Point pos0, Point pos1) = 0;

        virtual void DrawLine(uint16_t col, Point pos0, Point pos1) = 0;

        virtual void DrawChar(uint16_t col, char ch, Point pos, BitmapFont* font) = 0;
        virtual void DrawString(uint16_t col, const char* str, Point pos, BitmapFont* font) = 0;
};

#endif /* IDRAW_H_ */
