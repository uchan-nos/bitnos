/*
 * console.cpp
 *
 *  Created on: 2009/09/26
 *      Author: uchan
 */

#include <bitnos/console.h>
#include <bitnos/mtask.h>
#include <bitnos/sheet.h>
#include <bitnos/window.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bitnos/memory.h>
#include <bitnos/timer.h>
#include <bitnos/specialtask.h>
#include <bitnos/pit.h>
#include <bitnos/scancode.h>
#include <bitnos/calc.h>

namespace TaskConsole
{

    /* class ScreenBuffer */
    void ScreenBuffer::Init(int width, int height)
    {
        this->width = width;
        this->height = height;
        buf = new char[(this->width + 1) * this->height];

        begin = 0;
    }

    void ScreenBuffer::Dispose()
    {
        delete[] buf;
        buf = 0;
    }

    void ScreenBuffer::Write(Point pos, char ch)
    {
        if (IsAvailable(pos)) {
            pos = Calculate(pos);
            buf[pos.Y * (width + 1) + pos.X] = ch;
            buf[pos.Y * (width + 1) + pos.X + 1] = '\0';
        }
    }

    void ScreenBuffer::Write(Point pos, const char* str)
    {
        if (IsAvailable(pos)) {
            pos = Calculate(pos);
            char* p = buf + pos.Y * (width + 1) + pos.X;
            int x;
            for (x = pos.X; x < width && *str != '\0'; x++) {
                *p++ = *str++;
            }
            *p = '\0';
        }
    }

    char ScreenBuffer::Read(Point pos) const
    {
        if (IsAvailable(pos)) {
            pos = Calculate(pos);
            return buf[pos.Y * (width + 1) + pos.X];
        }
        return 0;
    }

    void ScreenBuffer::Scroll(int lines)
    {
        begin += lines;
        if (begin < 0) {
            begin += height;
        } else if (begin >= height) {
            begin -= height;
        }
    }

    const char* ScreenBuffer::GetPointer(Point pos) const
    {
        if (IsAvailable(pos)) {
            pos = Calculate(pos);
            return buf + pos.Y * (width + 1) + pos.X;
        }
        return 0;
    }

    bool ScreenBuffer::IsAvailable(Point pos) const
    {
        if (pos.X < 0 || width <= pos.X ||
                pos.Y < 0 || height <= pos.Y) {
            return false;
        }
        return true;
    }

    Point ScreenBuffer::Calculate(Point pos) const
    {
        pos.Y += begin;

        if (pos.Y >= height) {
            pos.Y -= height;
        }

        return pos;
    }
    /* end of class ScreenBuffer */


    // 定数
    const uint32_t TIMERDATA_CURSOR = 1;
    const uint32_t TIMERDATA_BEEP = 2;

    const int COMMAND_MAXLENGTH = 127; // コマンド最大文字数
    const int COMMAND_HISTORYSIZE = 32; // 履歴を記憶する最大数


    struct Valiables
    {
        Console cons;
        Image* img;
        Sheet* sht;

        char cmdline[COMMAND_MAXLENGTH + 1];
        int cmdlineBegin;

        Timer* tim;
        bool cursor;
        bool active;

        Timer* timBeep;
    };

    // コマンド実行機能なし
    void PutChar0(Valiables* o, char ch);

    // PutChar0 + コマンド実行
    void PutChar(Valiables* o, char ch);

    void PutString0(Valiables* o, const char* str);

    void PutString0(Valiables* o, const char* str, int len);

    void ConsoleRefreshScreen(Valiables* o, int begin, int printBegin = -1);

    void RunCommand(Valiables* o);

    void ShowCursor(Valiables* o);
    void EraseCursor(Valiables* o);

    void ConsoleMakeBorderLine(Image* img);

    void PrintHistoryUp(Valiables* o);
    void PrintHistoryDown(Valiables* o);

    void ClearInputString(Valiables* o);

    void ScrollUp(Valiables* o, int lines);
    void ScrollDown(Valiables* o, int lines);

    /*
     * コンソールコマンド関数
     */
    class ConsoleCommands
    {
        public:
            static void mem(Valiables* o, const char* arg);
            static void opencons(Valiables* o, const char* arg);
            static void echo(Valiables* o, const char* arg);
            static void beep(Valiables* o, const char* arg);
            static void eco(Valiables* o, const char* arg);
            static void cat(Valiables* o, const char* arg);
            static void calc(Valiables* o, const char* arg);
            static void rpn(Valiables* o, const char* arg);
            static void help(Valiables* o, const char* arg);
    };

    void Init(void* obj)
    {
        static int initialPosX = 110, initialPosY = 50;

        Valiables* o = (Valiables*)obj;

        Task* task = taskman->GetCurrentTask();


        // コンソールの大きさ設定
        o->cons.height = 12;
        o->cons.width = 34;

        // コンソールの色設定
        o->cons.backColor = ConvRGB16(0, 0, 0);
        o->cons.textColor = ConvRGB16(0xD0, 0xD0, 0xD0);

        // 各種初期化
        o->cmdlineBegin = 0;
        o->cursor = false;

        // コンソール画面がアクティブかどうか
        o->active = SpecialTask::Get(SpecialTask::Active) == task;

        // 文字列バッファの準備
        o->cons.sbuf.Init(o->cons.width, 50);

        // バッファ内の書き込み位置
        o->cons.wPos = Point(0, 0);

        // 画面内の表示位置
        o->cons.pPos = Point(0, 0);

        o->cons.rLine = 0;

        // コマンド履歴バッファ関連
        o->cons.History.buf.Init(COMMAND_MAXLENGTH, COMMAND_HISTORYSIZE);

        o->cons.History.wPos = Point(0, 0);
        o->cons.History.rPos = Point(0, 0);

        // 画面のバッファ
        o->img = new Image(
                8 + fontHankaku->GetWidth() * o->cons.width,
                25 + 8 + fontHankaku->GetHeight() * o->cons.height);
        Window::Make(o->img);
        o->img->DrawRectangleFill(
                o->cons.backColor, Point(0, 25), Point(o->img->GetWidth() - 1, o->img->GetHeight() - 1));
        ConsoleMakeBorderLine(o->img);

        o->sht = shtman->Alloc();
        o->sht->Init(o->img, Sheet::NoInvColor, Point(initialPosX, initialPosY));
        o->sht->SetParentTask(task);
        shtman->SetHeight(o->sht, 0, Sheet::Normal);

        // コンソールを開くごとに位置が変わるようにした。
        if (initialPosX < o->img->GetWidth() - 50 && initialPosY < o->img->GetHeight() - 50) {
            initialPosX += 20;
            initialPosY += 20;
        } else {
            initialPosX = 110;
            initialPosY = 50;
        }


        o->tim = timman->Alloc(task, TIMERDATA_CURSOR);
        o->tim->SetTime(80);

        o->timBeep = timman->Alloc(task, TIMERDATA_BEEP);

        PutChar0(o, '>');
    }

    void Proc(void* obj, Message* msg)
    {
        Valiables* o = (Valiables*)obj;

        if (msg->from == Message::From::Keyboard) {
            if (msg->arg != 0) {
                if (msg->arg < 0x80) {
                    EraseCursor(o);
                    if (o->cons.rLine < o->cons.wPos.Y - o->cons.height) {
                        o->cons.rLine = o->cons.wPos.Y - o->cons.height + 1;
                        if (o->cons.rLine < 0) {
                            o->cons.rLine = 0;
                        }
                        if (o->cons.wPos.Y < o->cons.height) {
                            o->cons.pPos.Y = o->cons.wPos.Y;
                        } else {
                            o->cons.pPos.Y = o->cons.height - 1;
                        }
                        ConsoleRefreshScreen(o, o->cons.rLine);
                    }
                    PutChar(o, msg->arg);
                    if (o->cursor) {
                        ShowCursor(o);
                    }
                } else if (msg->arg == (uint32_t)ScanCode::Keys::KeyArrowUp) {
                    PrintHistoryUp(o);
                } else if (msg->arg == (uint32_t)ScanCode::Keys::KeyArrowDown) {
                    PrintHistoryDown(o);
                } else if (msg->arg == (uint32_t)ScanCode::Keys::KeyPageUp) {
                    ScrollUp(o, o->cons.height - 2);
                } else if (msg->arg == (uint32_t)ScanCode::Keys::KeyPageDown) {
                    ScrollDown(o, o->cons.height - 2);
                }
            }
        } else if (msg->from == Message::From::Timer) {
            if (msg->arg == TIMERDATA_CURSOR) {
                if (o->active) {
                    o->tim->SetTime(70);
                    if (o->cursor) {
                        EraseCursor(o);
                        o->cursor = false;
                    } else {
                        ShowCursor(o);
                        o->cursor = true;
                    }
                }
            } else if (msg->arg == TIMERDATA_BEEP) {
                PIT::BeepOff();
            }
        } else if (msg->from == Message::From::System) {
            if (msg->arg == Message::System::WindowActivated) {
                o->active = true;
                o->tim->SetTime(20);
                ShowCursor(o);
            } else if (msg->arg == Message::System::WindowInactivated) {
                o->active = false;
                EraseCursor(o);
            }
        }
    }

    void NewLine(Valiables* o)
    {
        if (o->cons.wPos.Y < o->cons.sbuf.GetHeight() - 1) {
            o->cons.wPos.Y++;
        } else {
            o->cons.sbuf.Scroll(1);
            o->cons.sbuf.Write(Point(0, o->cons.sbuf.GetHeight() - 1), '\0');
            o->cmdlineBegin--;
        }
        if (o->cons.wPos.Y >= o->cons.height - 1) {
            o->cons.rLine = o->cons.wPos.Y - o->cons.height + 1;
            Debug::WriteLine("set rline to %d", o->cons.rLine);
        } else {
            o->cons.rLine = 0;
        }
        o->cons.wPos.X = 0;
        o->cons.pPos.X = 0;
        if (o->cons.pPos.Y < o->cons.height - 1) {
            o->cons.pPos.Y++;
        }
    }

    void PutChar0(Valiables* o, char ch)
    {
        if (0x20 <= ch && ch < 0x80) {
            o->cons.sbuf.Write(o->cons.wPos, ch);
        }
        if (ch == '\b') {
            if ((o->cons.wPos.Y != o->cmdlineBegin && o->cons.wPos.X > 0) ||
                    (o->cons.wPos.Y == o->cmdlineBegin && o->cons.wPos.X > 1)) {
                o->cons.wPos.X--;
                if (o->cons.pPos.X > 0) {
                    o->cons.pPos.X--;
                }
            } else if (o->cons.wPos.Y != o->cmdlineBegin && o->cons.wPos.X == 0) {
                o->cons.wPos.X = o->cons.sbuf.GetWidth() - 1;
                o->cons.wPos.Y--;
                o->cons.pPos.X = o->cons.width - 1;
                if (o->cons.pPos.Y > 0) {
                    o->cons.pPos.Y--;
                }
            }
            o->cons.sbuf.Write(o->cons.wPos, '\0');
            o->sht->DrawRectangleFill(
                    o->cons.backColor,
                    Point(
                        4 + fontHankaku->GetWidth() * o->cons.pPos.X,
                        25 + 4 + fontHankaku->GetHeight() * o->cons.pPos.Y),
                    Point(
                        4 + fontHankaku->GetWidth() * (o->cons.pPos.X + 1) - 1,
                        25 + 4 + fontHankaku->GetHeight() * (o->cons.pPos.Y + 1) - 1)
                    );
        } else if (ch == '\n' || o->cons.wPos.X >= o->cons.sbuf.GetWidth()) {
            int pposy = o->cons.pPos.Y;
            NewLine(o);

            if (pposy == o->cons.pPos.Y) {
                // スクロールしたとき
                //ConsoleRefreshScreen(o, -1);
                ConsoleRefreshScreen(o, o->cons.rLine);
            }

        } else {
            o->sht->DrawChar(
                    o->cons.textColor,
                    ch,
                    Point(
                        4 + fontHankaku->GetWidth() * o->cons.pPos.X,
                        25 + 4 + fontHankaku->GetHeight() * o->cons.pPos.Y),
                    fontHankaku);
            o->cons.wPos.X++;
            if (o->cons.pPos.X < o->cons.width) {
                o->cons.pPos.X++;
            }
        }
    }

    void PutChar(Valiables* o, char ch)
    {
        PutChar0(o, ch);
        if (ch == '\n') {
            int i = 0;
            char c;
            for (int line = o->cmdlineBegin, x = 1; line <= o->cons.wPos.Y; line++) {
                for (; (c = o->cons.sbuf.Read(Point(x, line))) != '\0'; x++) {
                    o->cmdline[i++] = c;
                    if (i == COMMAND_MAXLENGTH) {
                        break;
                    }
                }
                x = 0;
            }
            o->cmdline[i] = '\0';

            if (o->cmdline[0] != '\0') {
                // 履歴を保存
                o->cons.History.buf.Write(o->cons.History.wPos, o->cmdline);
                if (o->cons.History.wPos.Y < o->cons.History.buf.GetHeight() - 1) {
                    o->cons.History.wPos.Y++;
                } else {
                    o->cons.History.buf.Scroll(1);
                }

                RunCommand(o);
            }

            o->cmdlineBegin = o->cons.wPos.Y;
            PutChar0(o, '>');
        }
        o->cons.History.rPos = o->cons.History.wPos;
    }

    void PutString0(Valiables* o, const char* str)
    {
        while (*str != '\0') {
            PutChar0(o, *str++);
        }
    }

    void PutString0(Valiables* o, const char* str, int len)
    {
        while (len > 0 && *str != '\0') {
            PutChar0(o, *str++);
            len--;
        }
    }

    /*
     * begin : 文字列バッファ内の開始位置
     * printBegin : 画面内の開始位置
     */
    void ConsoleRefreshScreen(Valiables* o, int begin, int printBegin)
    {
        if (begin < 0) {
            // 自動計算モード
            begin = o->cons.wPos.Y - o->cons.height + 1;
            if (begin < 0) {
                begin = 0;
            }
        }

        if (printBegin < 0) {
            printBegin = 0;
        }

        // 画面全消去
        o->img->DrawRectangleFill(
                o->cons.backColor,
                Point(4, 25 + 4 + printBegin * fontHankaku->GetHeight()),
                Point(o->img->GetWidth() - 1 - 4, o->img->GetHeight() - 1 - 4));

        // 文字列描画
        const char* p = 0;
        for (int lines = printBegin; begin + lines <= o->cons.sbuf.GetHeight() && lines < o->cons.height; lines++) {
            p = o->cons.sbuf.GetPointer(Point(0, begin + lines));
            if (p == 0) {
                break;
            }
            o->img->DrawString(
                    o->cons.textColor,
                    p,
                    Point(4, 25 + 4 + lines * fontHankaku->GetHeight()),
                    fontHankaku);
        }
        o->sht->Refresh();
    }

    void RunCommand(Valiables* o)
    {
        char* command = o->cmdline;
        char* args;
        int i = 0;
        while (command[i] != ' ' && command[i] != '\0') {
            i++;
        }
        if (command[i] == ' ') {
            command[i] = '\0';
            i++;
            while (command[i] == ' ') {
                i++;
            }
        }
        args = command + i;
        if (strncmp(command, "mem", 4) == 0) {
            ConsoleCommands::mem(o, args);
        } else if (strncmp(command, "opencons", 9) == 0) {
            ConsoleCommands::opencons(o, args);
        } else if (strncmp(command, "echo", 5) == 0) {
            ConsoleCommands::echo(o, args);
        } else if (strncmp(command, "beep", 5) == 0) {
            ConsoleCommands::beep(o, args);
        } else if (strncmp(command, "eco", 4) == 0) {
            ConsoleCommands::eco(o, args);
        } else if (strncmp(command, "cat", 4) == 0) {
            ConsoleCommands::cat(o, args);
        } else if (strncmp(command, "calc", 5) == 0) {
            ConsoleCommands::calc(o, args);
        } else if (strncmp(command, "rpn", 4) == 0) {
            ConsoleCommands::rpn(o, args);
        } else if (strncmp(command, "help", 5) == 0) {
            ConsoleCommands::help(o, args);
        } else {
            if (strlen(command) > 0) {
                PutString0(o, "unknown command\n");
            }
        }
    }

    void ShowCursor(Valiables* o)
    {
        if (o->cons.pPos.X < 0 || o->cons.width < o->cons.pPos.X ||
                o->cons.pPos.Y < 0 || o->cons.height <= o->cons.pPos.Y) {
            return;
        }
        Point pos;
        pos = Point(4 + o->cons.pPos.X * fontHankaku->GetWidth(), 25 + 4 + o->cons.pPos.Y * fontHankaku->GetHeight());
        o->sht->DrawRectangleFill(
                o->cons.textColor,
                pos,
                pos + Point(0, fontHankaku->GetHeight() - 1));
    }

    void EraseCursor(Valiables* o)
    {
        if (o->cons.pPos.X < 0 || o->cons.width < o->cons.pPos.X ||
                o->cons.pPos.Y < 0 || o->cons.height <= o->cons.pPos.Y) {
            return;
        }
        Point pos;
        pos = Point(4 + o->cons.pPos.X * fontHankaku->GetWidth(), 25 + 4 + o->cons.pPos.Y * fontHankaku->GetHeight());
        o->sht->DrawRectangleFill(
                o->cons.backColor,
                pos,
                pos + Point(0, fontHankaku->GetHeight() - 1));
    }

    void ConsoleMakeBorderLine(Image* img)
    {
        Point pos0(1, 25);
        Point pos1(img->GetWidth() - 2, 25);
        Point pos2(1, img->GetHeight() - 2);
        Point pos3(img->GetWidth() - 2, img->GetHeight() - 2);
        img->DrawLine(
                ConvRGB16(0xc0, 0xc0, 0xc0),
                pos0, pos2);
        img->DrawLine(
                ConvRGB16(0xc0, 0xc0, 0xc0),
                pos1, pos3);
        img->DrawLine(
                ConvRGB16(0xc0, 0xc0, 0xc0),
                pos2 + Point(1, 0), pos3 + Point(-1, 0));
        img->DrawLine(
                ConvRGB16(0x80, 0x80, 0x80),
                pos0 + Point(1, 0), pos2 + Point(1, -2));
        img->DrawLine(
                ConvRGB16(0x80, 0x80, 0x80),
                pos0 + Point(2, 0), pos1 + Point(-1, 0));
        img->DrawLine(
                ConvRGB16(0xf0, 0xf0, 0xf0),
                pos1 + Point(-1, 1), pos3 + Point(-1, -1));
        img->DrawLine(
                ConvRGB16(0xf0, 0xf0, 0xf0),
                pos2 + Point(1, -1), pos3 + Point(-2, -1));
    }

    void ConsoleCommands::mem(Valiables* o, const char* arg)
    {
        char s[256];
        PutString0(o, "mem  : ");
        sprintf(s, "%10d", physicalMemorySize);
        if ((physicalMemorySize >> 30) != 0) {
            PutString0(o, s + 0, 1);
            PutChar0(o, ',');
        }
        if ((physicalMemorySize >> 20) != 0) {
            PutString0(o, s + 1, 3);
            PutChar0(o, ',');
        }
        if ((physicalMemorySize >> 10) != 0) {
            PutString0(o, s + 4, 3);
            PutChar0(o, ',');
        }
        PutString0(o, s + 7, 3);
        PutString0(o, " B\n");

        uint32_t freeSize = memman.GetFreeSize();
        PutString0(o, "free : ");
        sprintf(s, "%10d", freeSize);
        if ((freeSize >> 30) != 0) {
            PutString0(o, s + 0, 1);
            PutChar0(o, ',');
        }
        if ((freeSize >> 20) != 0) {
            PutString0(o, s + 1, 3);
            PutChar0(o, ',');
        }
        if ((freeSize >> 10) != 0) {
            PutString0(o, s + 4, 3);
            PutChar0(o, ',');
        }
        PutString0(o, s + 7, 3);
        PutString0(o, " B\n");
    }

    void ConsoleCommands::opencons(Valiables* o, const char* arg)
    {
        Task* consTask = taskman->Alloc();
        uintptr_t stackTop;
        uint32_t stackSize;
        stackTop = (uintptr_t)(new uint8_t[stackSize = 1024*4]);
        consTask->SetFuncs(stackTop, stackSize,
                TaskConsole::Init, DummyFunc, TaskConsole::Proc);
        consTask->InitQueue(128);
        taskman->Run(consTask, 1, 2);
    }

    void ConsoleCommands::echo(Valiables* o, const char* arg)
    {
        PutString0(o, arg);
        PutChar0(o, '\n');
    }

    void ConsoleCommands::beep(Valiables* o, const char* arg)
    {
        int cnt = 2712;
        const int freq = 1193180;
        if (arg[0] != '\0') {
            cnt = ((freq << 10) / strtol(arg, 0, 0) + 512) >> 10;
        }

        // freq = 1.19318MHz / cnt
        PIT::BeetSetCounter(cnt);
        PIT::BeepOn();

        // カウンタ2、スピーカイネーブル

        o->timBeep->SetTime(100);
    }

    void ConsoleCommands::eco(Valiables* o, const char* arg)
    {
        PutString0(o, "      tree      \n");
        PutString0(o, "  tree    tree  \n");
        PutString0(o, "tree  tree  tree\n");
    }

    void ConsoleCommands::cat(Valiables* o, const char* arg)
    {
        PutString0(o, " _                ___       _.--.\n");
        PutString0(o, " \\`.|\\..----...-'`   `-._.-'_.-'`\n");
        PutString0(o, " /  ' `         ,       __.--'\n");
        PutString0(o, " )/' _/     \\   `-_,   /\n");
        PutString0(o, " `-'\" `\"\\_  ,_.-;_.-\\_ ',\n");
        PutString0(o, "     _.-'_./   {_.'   ; /\n");
        PutString0(o, "    {_.-``-'         {_/\n");
    }

    void ConsoleCommands::calc(Valiables* o, const char* arg)
    {
        char s[32];
        sprintf(s, "%d\n", Calc::Calc(arg));
        PutString0(o, s);
    }

    void ConsoleCommands::rpn(Valiables* o, const char* arg)
    {
        char s[32];
        sprintf(s, "%d\n", Calc::RPNCalc(arg));
        PutString0(o, s);
    }

    void ConsoleCommands::help(Valiables* o, const char* arg)
    {
        /*
           static void mem(Valiables* o, const char* arg);
           static void opencons(Valiables* o, const char* arg);
           static void echo(Valiables* o, const char* arg);
           static void beep(Valiables* o, const char* arg);
           static void eco(Valiables* o, const char* arg);
           static void cat(Valiables* o, const char* arg);
           static void calc(Valiables* o, const char* arg);
           static void rpn(Valiables* o, const char* arg);
           */
        PutString0(o, "command list\n");
        PutString0(o, "mem : memory information\n");
        PutString0(o, "opencons : opens new console\n");
        PutString0(o, "echo arg : echoes argument\n");
        PutString0(o, "beep [freq] : beeps sound\n");
        PutString0(o, "eco : do an ecological act\n");
        PutString0(o, "cat [file]: print contents of file\n");
        PutString0(o, "calc exp : calculates exp\n");
        PutString0(o, "rpn : calculates exp in rpn\n");
    }

    void PrintHistoryUp(Valiables* o)
    {
        if (o->cons.History.rPos.Y >= 0) {
            if (o->cons.History.rPos.Y > 0) {
                o->cons.History.rPos.Y--;
            }
            ClearInputString(o);
            PutString0(o, o->cons.History.buf.GetPointer(o->cons.History.rPos));
        }
    }

    void PrintHistoryDown(Valiables* o)
    {
        if (o->cons.History.rPos.Y < o->cons.History.wPos.Y) {
            if (o->cons.History.rPos.Y < o->cons.History.wPos.Y - 1) {
                o->cons.History.rPos.Y++;
            }
            ClearInputString(o);
            PutString0(o, o->cons.History.buf.GetPointer(o->cons.History.rPos));
        }
    }

    void ClearInputString(Valiables* o)
    {
        o->cons.pPos.Y += o->cmdlineBegin - o->cons.wPos.Y;
        o->cons.pPos.X = 1;
        o->cons.wPos.Y = o->cmdlineBegin;
        o->cons.wPos.X = 1;
        o->cons.sbuf.Write(o->cons.wPos, '\0');
        ConsoleRefreshScreen(o, -1, o->cons.pPos.Y);
    }

    void ScrollUp(Valiables* o, int lines)
    {
        int oldRLine = o->cons.rLine;
        o->cons.rLine -= lines;
        if (o->cons.rLine < 0) {
            o->cons.rLine = 0;
        }
        Debug::WriteLine("new rline = %d, old = %d", o->cons.rLine, oldRLine);
        o->cons.pPos.Y += oldRLine - o->cons.rLine;
        ConsoleRefreshScreen(o, o->cons.rLine);
    }

    void ScrollDown(Valiables* o, int lines)
    {
        int oldRLine = o->cons.rLine;
        o->cons.rLine += lines;
        if (o->cons.rLine > o->cons.wPos.Y) {
            o->cons.rLine = o->cons.wPos.Y;
        }
        o->cons.pPos.Y += oldRLine - o->cons.rLine;
        ConsoleRefreshScreen(o, o->cons.rLine);
    }

}
