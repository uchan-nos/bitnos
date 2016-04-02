/*
 * debug.cpp
 *
 *  Created on: 2009/07/11
 *      Author: uchan
 */

#include <stdio.h>
#include <bitnos/debug.h>
#include <bitnos/point.h>

//Image* Debug::imageDebug;
Sheet* Debug::sheetDebug;
BitmapFont* Debug::font;
int Debug::line;
int Debug::lineMax;
int Debug::lineLength;

//void Debug::Init(Image* imageDebug, BitmapFont* font)
void Debug::Init(Sheet* sht, BitmapFont* font, int line, int len)
{
    //Debug::imageDebug = imageDebug;
    Debug::sheetDebug = sht;
    Debug::font = font;
    Debug::lineMax = line;
    Debug::lineLength = len;

    Debug::WriteLine("%08x", (uintptr_t)Debug::font);
}

void Debug::WriteLine(const char* format, ...)
{
    if (Debug::sheetDebug != 0) {
        //char s[1024];
        char s[128];
        va_list ap;
        va_start(ap, format);
        vsprintf(s, format, ap);
        va_end(ap);
        int lineOld = line - 1;
        if (lineOld < 0) {
            lineOld = lineMax - 1;
        }
        s[lineLength] = '\0';
        sheetDebug->DrawRectangleFill(
                sheetDebug->invColor,
                Point(0, lineOld * fontHankaku->GetHeight()),
                Point(fontHankaku->GetWidth() - 1, (lineOld + 1) * fontHankaku->GetHeight() - 1)
                );
        sheetDebug->DrawRectangleFill(
                sheetDebug->invColor,
                Point(8, line * fontHankaku->GetHeight()),
                Point(sheetDebug->image->GetWidth() - 1, (line + 1) * fontHankaku->GetHeight() - 1)
                );
        sheetDebug->DrawString(
                ConvRGB16(0xc0, 0xc0, 0xc0), "#", Point(0, line * fontHankaku->GetHeight()), fontHankaku);
        sheetDebug->DrawString(
                ConvRGB16(0xc0, 0xc0, 0xc0), s, Point(8, line * fontHankaku->GetHeight()), fontHankaku);
        line++;

        if (Debug::line >= Debug::lineMax) {
            Debug::line = 0;
        }
    }
}
