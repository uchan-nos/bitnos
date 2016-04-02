/*
 * mouse.cpp
 *
 *  Created on: 2009/08/01
 *      Author: uchan
 */

#include <bitnos/mouse.h>
#include <bitnos/int.h>
#include <bitnos/funcs.h>
#include <bitnos/kbc.h>
#include <bitnos/debug.h>
#include <bitnos/mtask.h>
#include <bitnos/point.h>
#include <bitnos/bootinfo.h>
#include <bitnos/sheet.h>
#include <bitnos/specialtask.h>

void inthandler2c(int* esp)
{
    // 割り込み確認をPICに通知
    PIC::Notify(12);

    // 割り込みハンドラ起動
    Mouse::IntHandler();
}

//Queue<Message>* Mouse::queue;
Task* Mouse::handleTask;
Mouse::Data Mouse::currentData;
Mouse::Type Mouse::mtype;
Mouse::RawData Mouse::buf;


//void Mouse::Init(Queue<Message>* queue)
void Mouse::Init(Task* handleTask)
{
    //Mouse::queue = queue;
    Mouse::handleTask = handleTask;

    // マウスの有効化
    KBC::SendToMouse(Command::EnableMouse);

    mtype = Normal3;
}

void Mouse::IntHandler()
{
    uint32_t data = KBC::ReceiveData();
    taskman->SendMessage(
            handleTask,
            Message(Message::From::Mouse, data)
            );
}

bool Mouse::ReceivedData(uint8_t data)
{
    static int phase = 0;

    bool complete = false;

    switch (phase) {
        case 0:
            if (data == 0xfa) {
                phase = 1;
            }
            break;
        case 1:
            if ((data & 0xc8) == 8) {
                // オーバーフローなし、ビット3がちゃんと1だった
                phase = 2;
                buf.Byte[0] = data;
            }
            break;
        case 2:
            phase = 3;
            buf.Byte[1] = data;
            break;
        case 3:
            switch (mtype) {
                case Wheel3:
                case Wheel5:
                    phase = 4;
                    break;
                default:
                    phase = 1;
                    complete = true;
                    break;
            }
            buf.Byte[2] = data;
            break;
        case 4:
            phase = 1;
            complete = true;
            buf.Byte[3] = data;
        default:
            break;
    }

    if (complete) {
        // 受信が全バイト完了した
        currentData.ButtonMiddle = buf.Detail.Button.Bit.Middle != 0;
        currentData.ButtonRight = buf.Detail.Button.Bit.Right != 0;
        currentData.ButtonLeft = buf.Detail.Button.Bit.Left != 0;
        currentData.Button5th = buf.Detail.Ext.Bit.Button5th != 0;
        currentData.Button4th = buf.Detail.Ext.Bit.Button4th != 0;
        currentData.XMove = buf.Detail.XMove;
        currentData.YMove = buf.Detail.YMove;
        currentData.ZMove = buf.Detail.Ext.Bit.ZMove;
        if (buf.Detail.Button.Bit.XSign) {
            currentData.XMove |= (-256);
        }
        if (buf.Detail.Button.Bit.YSign) {
            currentData.YMove |= (-256);
        }
        if (buf.Detail.Ext.Bit.ZSign) {
            currentData.ZMove |= (-8);
        }
    }

    return complete;
}


#define _B 0x0000
#define _O 0xffff
#define __ 0x0001
uint16_t mouseDefaultImage[] = {
    _B, _B, __, __, __, __, __, __, __, __,
    _B, _O, _B, __, __, __, __, __, __, __,
    _B, _O, _O, _B, __, __, __, __, __, __,
    _B, _O, _O, _O, _B, __, __, __, __, __,
    _B, _O, _O, _O, _O, _B, __, __, __, __,
    _B, _O, _O, _O, _O, _O, _B, __, __, __,
    _B, _O, _O, _O, _O, _O, _O, _B, __, __,
    _B, _O, _O, _O, _O, _O, _O, _O, _B, __,
    _B, _O, _O, _O, _O, _O, _O, _O, _O, _B,
    _B, _O, _O, _O, _O, _O, _B, _B, _B, _B,
    _B, _O, _O, _B, _O, _O, _B, __, __, __,
    _B, _O, _B, __, _B, _O, _O, _B, __, __,
    _B, _B, __, __, _B, _O, _O, _B, __, __,
    _B, __, __, __, _B, _O, _O, _B, __, __,
    __, __, __, __, __, _B, _O, _O, _B, __,
    __, __, __, __, __, _B, _O, _O, _B, __,
    __, __, __, __, __, _B, _B, _B, __, __,
};
#undef _B
#undef _O
#undef __

namespace TaskMouse
{
    struct Valiables
    {
        BootInfo* binfo;

        // マウスの座標
        Point mpos;

        // マウスの座標の変化量
        Point dmpos;

        // ウィンドウドラッグモード
        bool dragMode;

        // ドラッグモードになったときのマウスとウィンドウの相対座標
        Point dragRelPos;

        // ドラッグモードの対象のウィンドウ
        Sheet* dragSheet;

        // マウスの情報
        Mouse::Data mouseData, oldMouseData;

        // マウスポインタ
        Image* mimg;
        Sheet* msht;
    };

    void Init(void* obj)
    {
        Valiables* o = (Valiables*)obj;

        o->binfo = (BootInfo*)BOOTINFO_ADDR;

        // マウスの座標
        o->mpos = Point(o->binfo->xsize / 2, o->binfo->ysize / 2);

        // マウスの座標の変化量
        o->dmpos = Point(0, 0);

        // ウィンドウドラッグモード
        o->dragMode = false;

        // ドラッグモードの対象のウィンドウからみたマウスの座標
        o->dragRelPos = Point(0, 0);

        // ドラッグモードの対象のウィンドウ
        o->dragSheet = 0;

        // マウスの情報
        o->mouseData = Mouse::Data();

        // マウスポインタ
        o->mimg = new Image(mouseDefaultImage, MOUSE_DEFAULT_IMAGE_XSIZE, MOUSE_DEFAULT_IMAGE_YSIZE);
        o->msht = shtman->Alloc();
        o->msht->Init(o->mimg, 0x0001, o->mpos);
        shtman->SetHeight(o->msht, 0, Sheet::Mouse);
    }

    void PreProc(void* obj)
    {
        Valiables* o = (Valiables*)obj;

        Queue<Message>* queue = taskman->GetCurrentTask()->GetQueue();

        if (queue->GetCount() == 0 && o->dmpos != Point(0, 0)) {
            /*
             * CPU空き時間を見つけてウィンドウを移動させる
             * （ウィンドウドラッグ中にメッセージがすべて処理される
             * というのは、つまりCPUが多少暇だということ）
             */
            o->mpos += o->dmpos;
            o->dmpos = Point(0, 0);

            if (o->mpos.X < 0) {
                o->mpos.X = 0;
            } else if (o->mpos.X >= (int)o->binfo->xsize) {
                o->mpos.X = o->binfo->xsize - 1;
            }

            if (o->mpos.Y < 0) {
                o->mpos.Y = 0;
            } else if (o->mpos.Y >= (int)o->binfo->ysize) {
                o->mpos.Y = o->binfo->ysize - 1;
            }

            o->msht->Slide(o->mpos);

            if (o->dragMode) {
                o->dragSheet->Slide(o->mpos - o->dragRelPos);
            }
        }
    }

    void Proc(void* obj, Message* msg)
    {
        Valiables* o = (Valiables*)obj;

        if (msg->from == Message::From::Mouse) {
            if (Mouse::ReceivedData(msg->arg)) {
                o->oldMouseData = o->mouseData;
                o->mouseData = *Mouse::GetData();
                //const Mouse::Data* mdata = Mouse::GetData();

                //dmpos.X += mdata->XMove;
                //dmpos.Y -= mdata->YMove;
                o->dmpos.X += o->mouseData.XMove;
                o->dmpos.Y -= o->mouseData.YMove;

                if (!o->dragMode && !o->oldMouseData.ButtonLeft && o->mouseData.ButtonLeft) {
                    // 左クリックされた。マウスの下のシートを探す
                    o->dragSheet = shtman->GetTopSheet(o->mpos, Sheet::TopMost);

                    taskman->SendMessage(
                            SpecialTask::Get(SpecialTask::Active),
                            Message(
                                Message::From::System,
                                Message::System::WindowInactivated));
                    SpecialTask::Set(SpecialTask::Active, o->dragSheet->GetParentTask());
                    taskman->SendMessage(
                            o->dragSheet->GetParentTask(),
                            Message(
                                Message::From::System,
                                Message::System::WindowActivated));

                    if (!o->dragSheet->IsFixed()) {
                        shtman->MoveToTop(o->dragSheet);
                        if (o->dragSheet->pos.Y <= o->mpos.Y && o->mpos.Y <= o->dragSheet->pos.Y + 25) {
                            o->dragMode = true;
                            o->dragRelPos = o->mpos - o->dragSheet->pos;
                        }
                    }
                } else if (o->dragMode && !o->mouseData.ButtonLeft) {
                    o->dragMode = false;
                }
            }
        }
    }

}

