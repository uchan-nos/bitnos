/*
 * keyboard.cpp
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#include <bitnos/keyboard.h>
#include <bitnos/int.h>
#include <bitnos/funcs.h>
#include <bitnos/kbc.h>
#include <bitnos/mtask.h>
#include <bitnos/specialtask.h>
#include <bitnos/bootinfo.h>
#include <bitnos/keycode.h>
#include <bitnos/window.h>
#include <stdio.h>

void inthandler21(int* esp)
{
    // 割り込み確認をPICに通知
    PIC::Notify(1);

    // 割り込みハンドラ起動
    Keyboard::IntHandler();
}

//Queue<Message>* Keyboard::queue;
Task* Keyboard::handleTask;

//void Keyboard::Init(Queue<Message>* queue)
void Keyboard::Init(Task* handleTask)
{
    //Keyboard::queue = queue;
    Keyboard::handleTask = handleTask;

    // キーボードコントローラの初期化
    KBC::SetMode(
            KBC::Mode::UseScancode01 |
            KBC::Mode::EnableMouseIRQ |
            KBC::Mode::EnableKeyboardIRQ
            );
}

void Keyboard::IntHandler()
{
    uint8_t keycode = KBC::ReceiveData();
    taskman->SendMessage(
            handleTask,
            Message(Message::From::Keyboard, keycode)
            );
}

struct Command
{
    uint8_t commandBytes[4];
    int commandCount;
    uint8_t respondBytes[4];
    int respondCount;

    public:
    Command();
    Command(uint8_t command, int respondCount);
    Command(uint8_t command, int respondCount, uint8_t arg1);

};

Command::Command()
{
    commandCount = 0;
    respondCount = 0;
}

Command::Command(uint8_t command, int respondCount)
{
    commandCount = 1;
    commandBytes[0] = command;
    this->respondCount = respondCount;
}

Command::Command(uint8_t command, int respondCount, uint8_t arg1)
{
    commandCount = 2;
    commandBytes[0] = command;
    commandBytes[1] = arg1;
    this->respondCount = respondCount;

}

namespace TaskKeyboard
{
    struct Valiables
    {
        BootInfo* binfo;
        int c;

        char s[32];

        ScanCode* sc;
        ScanCode::Code code;

        // 0 0 0 0 0 0 L R
        uint8_t keyShift;
        uint8_t keyCtrl;
        uint8_t keyAlt;
        bool keyScrollLock;
        bool keyNumLock;
        bool keyCapsLock;

        uint8_t ledStatus;

        bool changeLedStatus;

        Queue<Command>* commands;
        Queue<uint8_t>* commandBytes;
        Queue<Command>* finishedCommands;

        // 次のバイトをキーボードに送ってもよいならtrue
        bool receiveAck;

        // 受信待ちバイト数
        int waitingByteCount;

        // キーボードから送られてきたデータ
        uint8_t rcv[8];

        uint8_t snd[8];

        // 現在のスキャンコード
        int currentScanCodeSet;

        Image* img;
        Sheet* sht;

        Image* img2;
        Sheet* sht2;
    };

    void Init(void* obj)
    {
        Valiables* o = (Valiables*)obj;

        o->binfo = (BootInfo*)BOOTINFO_ADDR;

        o->sc = new ScanCode01();

        o->keyShift = 0;
        o->keyCtrl = 0;
        o->keyAlt = 0;
        o->keyScrollLock = (o->binfo->leds & 0x10) != 0;
        o->keyNumLock = (o->binfo->leds & 0x20) != 0;
        o->keyCapsLock = (o->binfo->leds & 0x40) != 0;

        o->ledStatus = 0x00;

        o->changeLedStatus = true;

        o->commands = new Queue<Command>(8);
        o->commandBytes = new Queue<uint8_t>(16);
        o->finishedCommands = new Queue<Command>(8);

        // 次のバイトをキーボードに送ってもよいならtrue
        o->receiveAck = true;

        // 受信待ちバイト数
        o->waitingByteCount = -1;

        // 現在のスキャンコード
        o->currentScanCodeSet = 0x02;

        o->img = new Image(8 * 26, 16 * 3 + 25);
        Window::Make(o->img, "data from kb");
        o->sht = shtman->Alloc();
        o->sht->Init(o->img, Sheet::NoInvColor, Point(150, 130));
        shtman->SetHeight(o->sht, 0, Sheet::Normal);

        o->img2 = new Image(8 * 26, 16 * 2 + 25);
        Window::Make(o->img2, "data to kb");
        o->sht2 = shtman->Alloc();
        o->sht2->Init(o->img2, Sheet::NoInvColor, Point(170, 190));
        shtman->SetHeight(o->sht2, 0, Sheet::Normal);

    }

    void PreProc(void* obj)
    {
        Valiables* o = (Valiables*)obj;

        //const uint8_t keyModLeft = 0x02;
        //const uint8_t keyModRight = 0x01;

        // ロックキーに変化があるならLEDに反映させる
        if (o->changeLedStatus) {
            o->changeLedStatus = false;
            o->ledStatus =
                (o->keyScrollLock == true ? 0x01 : 0x00) |
                (o->keyNumLock == true ? 0x02 : 0x00) |
                (o->keyCapsLock == true ? 0x04 : 0x00);
            o->commands->PushBack(Command(KBC::Command::SetLEDs, 0, o->ledStatus));
        }

        if (o->waitingByteCount == 0) {
            o->finishedCommands->PushBack(o->commands->Front());
            o->commands->PopFront();
            o->waitingByteCount = -1;
        }

        if (o->finishedCommands->GetCount() > 0) {
            Debug::WriteLine("keyboard: cmd finished");
            if (o->finishedCommands->Front().commandBytes[0] == KBC::Command::SetScanCodeSet &&
                    o->finishedCommands->Front().commandBytes[1] == 0x00) {
                // スキャンコードの確認
                o->currentScanCodeSet = o->finishedCommands->Front().respondBytes[0];
                Debug::WriteLine("sc=%d", o->currentScanCodeSet);
            }
            o->finishedCommands->PopFront();
        }

        // キーボードに送るコマンドがあれば送る
        if (o->commands->GetCount() > 0 && o->commandBytes->GetCount() == 0 && o->waitingByteCount < 0) {
            Command& cmd = o->commands->Front();
            for (int i = 0; i < cmd.commandCount; i++) {
                o->commandBytes->PushBack(cmd.commandBytes[i]);
            }
        }
        if (o->commandBytes->GetCount() > 0 && o->receiveAck == true) {
            KBC::SendCommand(o->commandBytes->Front());
            o->receiveAck = false;
            for (int i = sizeof(o->snd) - 1; i >= 1; i--) {
                o->snd[i] = o->snd[i - 1];
            }
            o->snd[0] = o->commandBytes->Front();

            o->sht2->DrawRectangleFill(
                    ConvRGB16(0xe0, 0xe0, 0xe0),
                    Point(8 + 8 * 0, 25 + 8),
                    Point(8 + 8 * 24 - 1, 25 + 24 - 1)
                    );
            sprintf(o->s, "%02x>%02x>%02x>%02x>%02x>%02x>%02x>%02x",
                    o->snd[0], o->snd[1], o->snd[2], o->snd[3],
                    o->snd[4], o->snd[5], o->snd[6], o->snd[7]);
            o->sht2->DrawString(
                    ConvRGB16(0x000000), o->s, Point(8, 25 + 8), fontHankaku);
        }
    }

    void Proc(void* obj, Message* msg)
    {
        Valiables* o = (Valiables*)obj;

        const uint8_t keyModLeft = 0x02;
        const uint8_t keyModRight = 0x01;

        for (int i = sizeof(o->rcv) - 1; i >= 1; i--) {
            o->rcv[i] = o->rcv[i - 1];
        }
        o->rcv[0] = msg->arg;

        o->sht->DrawRectangleFill(
                ConvRGB16(0xe0, 0xe0, 0xe0),
                Point(8 + 8 * 0, 25 + 8),
                Point(8 + 8 * 24 - 1, 25 + 24 - 1)
                );
        sprintf(o->s, "%02x>%02x>%02x>%02x>%02x>%02x>%02x>%02x",
                o->rcv[0], o->rcv[1], o->rcv[2], o->rcv[3],
                o->rcv[4], o->rcv[5], o->rcv[6], o->rcv[7]);
        o->sht->DrawString(
                ConvRGB16(0x000000), o->s, Point(8, 25 + 8), fontHankaku);

        if (msg->from == Message::From::Keyboard) {
            if (o->waitingByteCount > 0) {
                Command& cmd = o->commands->Front();
                cmd.respondBytes[cmd.respondCount - o->waitingByteCount] = msg->arg;
                o->waitingByteCount--;
            } else {
                if (o->rcv[0] == 0xfa) {
                    o->receiveAck = true;
                    o->commandBytes->PopFront();
                    if (o->commandBytes->GetCount() == 0) {
                        o->waitingByteCount = o->commands->Front().respondCount;
                    }
                } else if (o->rcv[0] == 0xfe) {
                    o->receiveAck = true;
                } else {

                    o->code = o->sc->Convert(o->rcv);
                    o->c = o->sc->ToAscii(o->code, o->keyShift != 0);

                    if ((o->code.code & ScanCode::Keys::FlagBreak) == 0 && o->code.code != 0) {

                        o->sht->DrawRectangleFill(
                                ConvRGB16(0xe0, 0xe0, 0xe0),
                                Point(8 + 8 * 0, 25 + 8 + 16),
                                Point(8 + 8 * 24 - 1, 25 + 24 + 16 - 1)
                                );
                        sprintf(o->s, "keycode = 0x%02x", o->code.code);
                        o->sht->DrawString(
                                ConvRGB16(0x000000), o->s, Point(8, 25 + 16 + 8), fontHankaku);

                    }

                    if (o->c == 0) {
                        switch (o->code.code) {
                            case ScanCode::Keys::KeyCapsLock:
                                o->keyCapsLock = !o->keyCapsLock;
                                o->changeLedStatus = true;
                                break;
                            case ScanCode::Keys::KeyNumLock:
                                o->keyNumLock = !o->keyNumLock;
                                o->changeLedStatus = true;
                                break;
                            case ScanCode::Keys::KeyScrollLock:
                                o->keyScrollLock = !o->keyScrollLock;
                                o->changeLedStatus = true;
                                break;
                            case ScanCode::Keys::KeyRightShift:
                                o->keyShift |= keyModRight;
                                break;
                            case ScanCode::Keys::KeyLeftShift:
                                o->keyShift |= keyModLeft;
                                break;
                            case ScanCode::Keys::KeyRightShift + ScanCode::Keys::FlagBreak:
                                o->keyShift &= ~keyModRight;
                                break;
                            case ScanCode::Keys::KeyLeftShift + ScanCode::Keys::FlagBreak:
                                o->keyShift &= ~keyModLeft;
                                break;
                        }
                        o->c = o->code.code;
                    }// else {
                    if ('A' <= o->c && o->c <= 'Z') {
                        if ((!o->keyCapsLock && o->keyShift == 0) ||
                                (o->keyCapsLock && o->keyShift != 0)) {
                            o->c += 'a' - 'A';
                        }
                    }
                    taskman->SendMessage(
                            SpecialTask::Get(SpecialTask::Active),
                            Message(
                                Message::From::Keyboard,
                                o->c
                                )
                            );
                    //}
                }
            }
        }
    }

}

