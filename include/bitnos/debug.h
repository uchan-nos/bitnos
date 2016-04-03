/*
 * debug.h
 *
 *  Created on: 2009/07/11
 *      Author: uchan
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <bitnos/font.h>
#include <bitnos/sheet.h>

/*
 * デバッグウィンドウ
 */
class Debug
{
    //static Image* imageDebug;
    static Sheet* sheetDebug;
        static BitmapFont* font;
        static int line;
        static int lineMax;
        static int lineLength;
    public:
           //static void Init(Image* imageDebug, BitmapFont* font);
           static void Init(Sheet* sht, BitmapFont* font, int line, int len);
               static void WriteLine(const char* format, ...);
};


#endif /* DEBUG_H_ */
