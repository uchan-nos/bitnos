/*
 * boot.cpp
 *
 *  Created on: 2009/07/05
 *      Author: uchan
 */

#include <bitnos/bitnos.h>
#include <stdio.h>
#include <string.h>

#include <bitnos/gui/Window.h>
#include <bitnos/gui/Label.h>
#include <bitnos/gui/TextBox.h>

class MainWindow : public GUI::Window
{
    protected:
        GUI::Control* controls0[4];
        //GUI::Label label1;
        GUI::TextBox textBox1;
        char label1Buf[32];

        int inputXPos;

        bool cursor;
    public:
        MainWindow(Sheet* sht) :
            Window(controls0, sht),
            //label1(Point(8, 8 + 25), label1Buf, 32, fontHankaku),
            textBox1(Point(8, 8 + 25), label1Buf, 27, fontHankaku, ConvRGB16(0xf0, 0xf0, 0xf0)),
            inputXPos(0), cursor(false)
    {
        //controls[0] = &label1;
        controls[0] = &textBox1;
        numControl = 1;
    }

        virtual ~MainWindow()
        {}

        void EventInputChar(char c)
        {
            if (c != 0) {
                if (c == '\b') {
                    if (inputXPos > 0) {
                        inputXPos--;
                        label1Buf[inputXPos] = '\0';
                        Render(
                                Point(8 + 2 + (inputXPos + 0) * fontHankaku->GetWidth(), 8 + 25 + 2),
                                Point(8 + 2 + (inputXPos + 2) * fontHankaku->GetWidth() - 1, 8 + 25 + 2 + 16 - 1));
                    }
                } else if (inputXPos < 26) {
                    label1Buf[inputXPos] = c;
                    label1Buf[inputXPos + 1] = '\0';
                    inputXPos++;
                    Render(
                            Point(8 + 2 + (inputXPos - 1) * fontHankaku->GetWidth(), 8 + 25 + 2),
                            Point(8 + 2 + inputXPos * fontHankaku->GetWidth() - 1, 8 + 25 + 2 + 16 - 1));
                }
            }
        }

        void EventTimerTimeUp()
        {
            if (cursor) {
                textBox1.ShowCursor();
                cursor = false;
            } else {
                textBox1.ClearCursor();
                cursor = true;
            }
            Render(
                    Point(8 + 2 + (inputXPos + 0) * fontHankaku->GetWidth(), 8 + 25 + 2),
                    Point(8 + 2 + (inputXPos + 1) * fontHankaku->GetWidth() - 1, 8 + 25 + 2 + 16 - 1));
        }

        void ClearCursor()
        {
            textBox1.ClearCursor();
            cursor = false;
            Render(
                    Point(8 + 2 + (inputXPos + 0) * fontHankaku->GetWidth(), 8 + 25 + 2),
                    Point(8 + 2 + (inputXPos + 1) * fontHankaku->GetWidth() - 1, 8 + 25 + 2 + 16 - 1));
        }

        void ShowCursor()
        {
            textBox1.ShowCursor();
            cursor = true;
            Render(
                    Point(8 + 2 + (inputXPos + 0) * fontHankaku->GetWidth(), 8 + 25 + 2),
                    Point(8 + 2 + (inputXPos + 1) * fontHankaku->GetWidth() - 1, 8 + 25 + 2 + 16 - 1));
        }

};

void RenderBackground(Image* vram);

int testvar1;
int testvar2 = 100;

BitmapFont fontHankaku0;

void BNosMain()
{
    BootInfo* binfo = (BootInfo*)0x0f00;
    Image imageVram((uint16_t*)binfo->vramaddr, binfo->xsize, binfo->ysize);

    char s[128];



    Point mpos; // マウス座標
    mpos = Point(binfo->xsize / 2, binfo->ysize / 2);
    Point dmpos = Point(0, 0); // マウス座標の変化分

    // グローバルディスクリプタテーブルの初期化
    GDT::Init();

    // 割り込みディスクリプタテーブルの初期化
    IDT::Init();

    // メモリ容量の取得
    physicalMemorySize = GetPhysicalMemoryCapacity(0x01200000, 0xc0000000);

    // メモリ管理情報は0x00300000 - 0x003c0000 (768KB)
    //memman.Init(0x01200000, memsize, (MemInfo*)0x00300000);
    memman.Init(0x01200000, physicalMemorySize - 0x01200000);

    // メッセージキュー
    Queue<Message> queueMain(128);

    // シートマネージャの初期化
    shtman = new SheetManager(&imageVram);

    // 背景のシート
    Image* imagebg = new Image(imageVram.GetWidth(), imageVram.GetHeight());
    RenderBackground(imagebg);

    Sheet* sheetbg = shtman->Alloc();
    sheetbg->Init(imagebg, Sheet::NoInvColor, Point(0, 0));
    shtman->SetHeight(sheetbg, 0, Sheet::Background);

    /*
    // マウスのシート
    Image* imagem = new Image(mouseDefaultImage, MOUSE_DEFAULT_IMAGE_XSIZE, MOUSE_DEFAULT_IMAGE_YSIZE);

    Sheet* sheetm = shtman->Alloc();
    sheetm->Init(imagem, 0x0001, mpos);
    shtman->SetHeight(sheetm, 0, Sheet::Mouse);
    */

    // フォントの準備
    fontHankaku = &fontHankaku0;
    fontHankaku->Init(8, 16, _binary_hankaku_bin_start, (int)_binary_hankaku_bin_size, 16);

    // デバッグのシート
    Image* imagedebug = new Image(8 * 40, 16 * 30);
    imagedebug->DrawRectangleFill(0x0001, Point(0, 0), Point(8 * 40 - 1, 16 * 30 - 1));

    Sheet* sheetdebug = shtman->Alloc();
    sheetdebug->Init(imagedebug, 0x0001, Point(0, 0));
    shtman->SetHeight(sheetdebug, 1, Sheet::Background);

    // シートのテスト
    Image* imagetest = new Image(240, 100);
    Window::Make(imagetest, "window of boot.cpp");
    //imagetest->DrawRectangle(ConvRGB16(0, 0, 0), Point(6, 30), Point(233, 49));
    /*
       imagetest->DrawLine(ConvRGB16(0x80, 0x80, 0x80), Point(6, 30), Point(232, 30));
       imagetest->DrawLine(ConvRGB16(0x80, 0x80, 0x80), Point(6, 30), Point(6, 49));
       imagetest->DrawLine(ConvRGB16(0xff, 0xff, 0xff), Point(7, 49), Point(233, 49));
       imagetest->DrawLine(ConvRGB16(0xff, 0xff, 0xff), Point(233, 30), Point(233, 49));
       imagetest->DrawRectangleFill(ConvRGB16(0xf0, 0xf0, 0xf0), Point(7, 31), Point(232, 48));
       */

    Sheet* sheettest = shtman->Alloc();
    sheettest->Init(imagetest, Sheet::NoInvColor, Point(500, 300));
    //sheettest->Init(imagetest, 0x0001, Point(500, 300));
    //shtman->SetHeight(sheettest, 100, Sheet::Normal);
    shtman->SetHeight(sheettest, 100, Sheet::Normal);

    // デバッグ画面の初期化
    Debug::Init(sheetdebug, fontHankaku, 30, 40);

    // 割り込みコントローラの初期化
    PIC::Init();
    io_sti();


    //Debug::WriteLine("queueMain.buf = %08x", (uintptr_t)queueMain.buf);
    Debug::WriteLine("&queueMain = %08x", (uintptr_t)&queueMain);

    // タイマーマネージャの初期化
    timman = new TimerManager();

    // タスクマネージャの初期化
    taskman = new TaskManager();

    Task* task = taskman->GetCurrentTask();
    task->InitQueue(&queueMain);

    SpecialTask::Set(SpecialTask::Active, task);

    // キーボードとマウスの初期化
    //Keyboard::Init(&queueMain);
    Keyboard::Init(SpecialTask::Get(SpecialTask::Keyboard));
    //Mouse::Init(&queueMain);
    Mouse::Init(SpecialTask::Get(SpecialTask::Mouse));

    Debug::WriteLine("taskman=%08x", (uint32_t)taskman);

    //Debug::WriteLine("taskman init");

    // PITの初期化
    PIT::Init();

    //Debug::WriteLine("pit init");

    // 割り込みの許可
    PIC::SetMask(PIC::IRQ0 | PIC::IRQ1 | PIC::IRQ2 | PIC::IRQ12);

    // タスクスケジューリング開始
    taskman->StartScheduling();

    Debug::WriteLine("mem = %d MB, %d B", physicalMemorySize >> 20, physicalMemorySize);

    /*
       for (int i = 0; i <= shtman->top; i++) {
       Debug::WriteLine("%2d:sid=%d,t=%d,h=%d",
       i, shtman->sheets[i] - shtman->sheets0, shtman->sheets[i]->type, shtman->sheets[i]->height);
       }
       */

    Timer* tim1 = timman->Alloc(task, 1);
    tim1->SetTime(50);

    sheettest->SetParentTask(taskman->GetCurrentTask());

    Message* msg;

    MainWindow mainWindow1(sheettest);
    mainWindow1.Render();

    Timer* timCursor = timman->Alloc(task, 2);
    timCursor->SetTime(50);

    bool active = SpecialTask::Get(SpecialTask::Active) == task;

    for (;;) {
        io_cli();
        if (queueMain.GetCount() == 0) {
            taskman->Sleep(task);
            io_sti();
        } else {
            io_sti();
            msg = &queueMain.Front();
            if (msg->from == Message::From::Keyboard) {
                //s[0] = ScanCodeUtils::ToAscii(msg->arg);
                s[0] = msg->arg;

                //*
                if (s[0] != 0) {
                    mainWindow1.EventInputChar(s[0]);
                }
                //*/

                /*
                   if (s[0] != 0) {
                   if (s[0] == '\b') {
                   if (inputXPos > 0) {
                   inputXPos--;
                   sheettest->DrawRectangleFill(
                   ConvRGB16(0xf0, 0xf0, 0xf0),
                   Point(8 + 8 * inputXPos, 32),
                   Point(8 + 8 + 8 * inputXPos - 1, 32 + 16 - 1)
                   );
                   }
                   } else if (inputXPos < 28) {
                   s[1] = '\0';
                   sheettest->DrawString(
                   ConvRGB16(0x0000ff), s, Point(8 + 8 * inputXPos, 32), fontHankaku);
                   inputXPos++;
                   }
                   }
                //*/
            } else if (msg->from == Message::From::Timer) {
                if (msg->arg == 1) {
                    //tim1->SetTime(500);
                    Task* consTask = taskman->Alloc();
                    uintptr_t stackTop;
                    uint32_t stackSize;
                    stackTop = (uintptr_t)(new uint8_t[stackSize = 1024*8]);
                    consTask->SetFuncs(stackTop, stackSize,
                            TaskConsole::Init, DummyFunc, TaskConsole::Proc);
                    consTask->InitQueue(128);
                    taskman->Run(consTask, 1, 2);
                } else if (msg->arg == 2) {
                    //Debug::WriteLine("timCursor timeup");
                    if (active) {
                        timCursor->SetTime(50);
                        timCursor->SetTime(50);
                        mainWindow1.EventTimerTimeUp();
                    }
                }
            } else if (msg->from == Message::From::System) {
                if (msg->arg == Message::System::WindowActivated) {
                    mainWindow1.ShowCursor();
                    active = true;
                    timCursor->SetTime(50);
                } else if (msg->arg == Message::System::WindowInactivated) {
                    mainWindow1.ClearCursor();
                    active = false;
                }
            }
            io_cli();
            queueMain.PopFront();
            io_sti();
        }
    }
}

void RenderBackground(Image* vram)
{

    int tb_h = 35;
    int w = vram->GetWidth();
    int h = vram->GetHeight();

    vram->DrawRectangleFill(ConvRGB16(47, 54, 153),
            0, 0, vram->GetWidth() - 1, h - tb_h - 1);

    /*
       vram->DrawRectangleFill(ConvRGB16(0x808080),
       0, h - 30, vram->GetWidth() - 1, h - 29);

       vram->DrawRectangleFill(ConvRGB16(0x808080),
       0, h - 2, vram->GetWidth() - 1, h - 1);

       vram->DrawRectangleFill(ConvRGB16(0x808080),
       0, h - 28, 1, h - 3);

       vram->DrawRectangleFill(ConvRGB16(0x808080),
       vram->GetWidth() - 2, h - 28, vram->GetWidth() - 1, h - 3);
       */

    /*
       vram->DrawRectangleFill(ConvRGB16(0xc0c0c0),
       2, h - 28, vram->GetWidth() - 2, h - 2);
       */
    vram->DrawRectangleFill(ConvRGB16(0xc0c0c0),
            0, h - tb_h, vram->GetWidth() - 1, h - 1);

    /*
       vram->DrawRectangleFill(ConvRGB16(0x808080),
       10, h - tb_h + 5, 60 - 1, h - 5 - 1);
       */

    int xpos0, ypos0, xpos1, ypos1;
    xpos0 = 10;
    ypos0 = h - tb_h + 5;
    xpos1 = 80 - 1;
    ypos1 = h - 3 - 1;
    vram->DrawRectangleFill(ConvRGB16(0xffffff),
            xpos0, ypos0, xpos1, ypos0);
    vram->DrawRectangleFill(ConvRGB16(0xffffff),
            xpos0, ypos0, xpos0, ypos1 - 1);
    vram->DrawRectangleFill(ConvRGB16(0x606060),
            xpos0, ypos1, xpos1, ypos1);
    vram->DrawRectangleFill(ConvRGB16(0x606060),
            xpos1, ypos0 + 1, xpos1, ypos1);

    xpos0 = w - 150;
    ypos0 = h - tb_h + 3;
    xpos1 = w - 10;
    ypos1 = h - 3 - 1;
    vram->DrawRectangleFill(ConvRGB16(0x606060),
            xpos0, ypos0, xpos1, ypos0);
    vram->DrawRectangleFill(ConvRGB16(0x606060),
            xpos0, ypos0, xpos0, ypos1 - 1);
    vram->DrawRectangleFill(ConvRGB16(0xffffff),
            xpos0, ypos1, xpos1, ypos1);
    vram->DrawRectangleFill(ConvRGB16(0xffffff),
            xpos1, ypos0 + 1, xpos1, ypos1);
}
