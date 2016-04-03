/*
 * console.h
 *
 *  Created on: 2009/09/26
 *      Author: uchan
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>
#include <bitnos/point.h>

namespace TaskConsole
{
    // charの2次元配列リングバッファ
    class ScreenBuffer
    {
        char* buf;
        int width, height;
        int begin; // データの先頭位置

        public:
        void Init(int width, int height);

        void Dispose();

        void Write(Point pos, char ch);

        void Write(Point pos, const char* str);

        char Read(Point pos) const;

        const char* GetPointer(Point pos) const;

        void Scroll(int lines);

        int GetWidth() const
        {
            return width;
        }

        int GetHeight() const
        {
            return height;
        }

        private:
        bool IsAvailable(Point pos) const;

        Point Calculate(Point pos) const;
    };


    struct Console
    {
        int width; // 横の文字数
        int height; // 縦の文字数
        //int bufWidth; // バッファの横の文字数
        //int bufHeight; // バッファの縦の文字数
        //char* buf; // バッファ

        //int startLine; // バッファの開始行
        //int endLine; // バッファの終了行(書き込み行)
        //int xPos; // 書き込みの横位置
        //int printYPos; // 表示の縦位置

        uint16_t backColor; // 背景色
        uint16_t textColor; // 前景色

        ScreenBuffer sbuf;
        Point wPos; // 書き込みの位置
        Point pPos; // 表示の位置

        int rLine; // バッファ内の表示開始行

        struct {
            ScreenBuffer buf; // コマンド履歴バッファ
            Point wPos; // 書き込みの位置
            Point rPos; // 読み込みの位置
        } History;

    };
}

class Message;

namespace TaskConsole
{
    void Init(void* obj);
    void Proc(void* obj, Message* msg);
}


#endif /* CONSOLE_H_ */
