/*
 * mtask.cpp
 *
 *  Created on: 2009/08/12
 *      Author: uchan
 */

#include <bitnos/mtask.h>
#include <bitnos/funcs.h>
#include <bitnos/debug.h>
#include <bitnos/int.h>
#include <stdio.h>
#include <bitnos/specialtask.h>
#include <bitnos/keyboard.h>
#include <bitnos/mouse.h>
#include <stdint.h>

#include <bitnos/window.h>
#include <bitnos/keycode.h>

#include <bitnos/TaskFuncs.h>

void Task::InitQueue(int size)
{
    queue = new Queue<Message>(size);
}

void Task::InitQueue(Queue<Message>* queue)
{
    this->queue = queue;
}

void Task::DisposeQueue()
{
    delete queue;
}

TaskManager::TaskManager()
{
    int i;
    uintptr_t stackTop;
    uint32_t stackSize;
    SegmentDescriptor* gdt = (SegmentDescriptor*)SEGMDESCTBL_ADDR;

    /*
     * レベルキューの初期化
     */
    for (i = 0; i < MaxLevels; i++) {
        taskLevel[i] = new Queue<Task*>(MaxLevelTasks);
    }

    /*
     * タスク構造体の初期化とGDTの設定
     */
    for (i = 0; i < MaxTasks; i++) {
        tasks0[i].next = tasks0 + i + 1;
        tasks0[i].selector = i + TSSBegin;
        gdt[i + TSSBegin].Init(sizeof(TSS32) - 1, (uint32_t)&tasks0[i].tss, TYPE_TSS32, 0, 0);
    }
    tasks0[MaxTasks - 1].next = 0;

    taskInvalidBegin = tasks0;

    // 現在動いているタスクは一番最初に追加
    Task* task = Alloc();
    load_tr(task->selector * 8);
    Run(task, 2, 3);
    currentLevel = 3;

    Task* taskIdle = Alloc();
    stackTop = (uintptr_t)(new uint8_t[stackSize = 1024]);
    SetTaskFuncs(&taskIdle->tss, stackTop, stackSize,
            DummyFunc, DummyFunc, TaskIdle::Proc);
    Run(taskIdle, 1, MaxLevels - 1);

    Task* taskKeyboard = Alloc();
    stackTop = (uintptr_t)(new uint8_t[stackSize = 1024*4]);
    SetTaskFuncs(&taskKeyboard->tss, stackTop, stackSize,
            TaskKeyboard::Init, TaskKeyboard::PreProc, TaskKeyboard::Proc);
    taskKeyboard->InitQueue(128);
    Run(taskKeyboard, 1, 2);

    Task* taskMouse = Alloc();
    stackTop = (uintptr_t)(new uint8_t[stackSize = 1024*4]);
    SetTaskFuncs(&taskMouse->tss, stackTop, stackSize,
            TaskMouse::Init, TaskMouse::PreProc, TaskMouse::Proc);
    taskMouse->InitQueue(128);
    Run(taskMouse, 3, 1);

    Task* taskTest = Alloc();
    stackTop = (uintptr_t)(new uint8_t[stackSize = 1024*4]);
    SetTaskFuncs(&taskTest->tss, stackTop, stackSize,
            TaskTest::Init, DummyFunc, TaskTest::Proc);
    Run(taskTest, 2, 2);


    SpecialTask::Set(SpecialTask::Keyboard, taskKeyboard);
    SpecialTask::Set(SpecialTask::Mouse, taskMouse);

    changeLevel = true;

    taskTimer = timman->Alloc(0, 0);
}

void TaskManager::StartScheduling()
{
    /*
       taskTimer->SetTime(
       taskLevel[currentLevel]->Front()->time
       );
       */
    Switch();
}

Task* TaskManager::Alloc()
{
    DINT
        Task* task = taskInvalidBegin;
    if (task != 0) {
        // 線形リストの設定
        taskInvalidBegin = task->next;
        task->next = 0;
    }
    RINT
        if (task != 0) {
            // 各種パラメタ
            task->flag = Task::Inactive;
            task->queue = 0;
            task->time = 1;
            task->level = MaxLevels / 2;

            // TSSの設定
            task->tss.ldtr = 0;
            task->tss.iomap = 0x40000000;
            task->tss.eflags = 0x00000202;
            task->tss.eax = 0;
            task->tss.ecx = 0;
            task->tss.edx = 0;
            task->tss.ebx = 0;
            task->tss.ebp = 0;
            task->tss.esi = 0;
            task->tss.edi = 0;
            task->tss.cs = 2 * 8;
            task->tss.es = 1 * 8;
            task->tss.ss = 1 * 8;
            task->tss.ds = 1 * 8;
            task->tss.fs = 1 * 8;
            task->tss.gs = 1 * 8;
        }
    return task;
}

void TaskManager::Free(Task* task)
{
    if (task != 0) {
        DINT
            task->next = taskInvalidBegin;
        taskInvalidBegin = task;
        RINT

            task->flag = Task::Invalid;
    }
}

void TaskManager::Switch()
{
    // タスクキューの操作
    DINT
        Task* currentTask = taskLevel[currentLevel]->Front();
    if (taskLevel[currentLevel]->GetCount() >= 2) {
        taskLevel[currentLevel]->PopFront();
        taskLevel[currentLevel]->PushBack(currentTask);
    }
    RINT

        // 次のタスクの取得
        if (changeLevel) {
            SearchCurrentLevel();
            changeLevel = false;
        }
    Task* nextTask = taskLevel[currentLevel]->Front();

    // タスクの実行時間を設定
    taskTimer->SetTime(
            nextTask->time
            );

    // タスク切り替え
    if (currentTask != nextTask) {
        if (nextTask->level >= MaxLevels) {
            for (;;);
        }
        farjmp(0, nextTask->selector * 8);
    }
}

void TaskManager::Run(Task* task, int time, int level)
{
    if (task == 0) {
        return;
    }

    if (level < 0) {
        // levelが負なら優先レベルを変えない
        level = task->level;
    } else if (level >= MaxLevels) {
        level = MaxLevels - 1;
    }
    if (time > 0) {
        task->time = time;
    }
    DINT
        if (task->flag == Task::Active && task->level != level) {
            // 動作中のレベル変更
            taskLevel[task->level]->Remove(task);
            task->flag = Task::Inactive;
        }
    if (task->flag == Task::Inactive && taskLevel[level]->GetFree() > 0) {
        task->level = level;
        taskLevel[task->level]->PushBack(task);
        task->flag = Task::Active;
    }
    RINT

        changeLevel = true;
}

void TaskManager::Sleep(Task* task)
{
    DINT
        if (task != 0 &&
                0 <= task->level && task->level < MaxLevels &&
                task->flag == Task::Active) {
            bool ts = false;
            if (task == GetCurrentTask()) {
                // 後でタスクを切り替える
                ts = true;
            }
            bool result = taskLevel[task->level]->Remove(task);
            if (result == false) {
                goto end;
            }
            task->flag = Task::Inactive;

            // currentLevelを再設定する
            SearchCurrentLevel();

            if (ts) {
                Task* nextTask = taskLevel[currentLevel]->Front();
                farjmp(0, nextTask->selector * 8);
            }
        }
end:
    RINT
}

void TaskManager::SendMessage(Task* task, const Message& msg)
{
    if (task != 0 && task->queue != 0) {
        task->queue->PushBack(msg);
        if (task->flag == Task::Inactive) {
            Run(task, task->time, task->level);
        }
    }
}

Task* TaskManager::GetCurrentTask()
{
    Task* task = taskLevel[currentLevel]->Front();
    return task;
}

void TaskManager::SearchCurrentLevel()
{
    int i;

    for (i = 0; i < MaxLevels; i++) {
        if (taskLevel[i]->GetCount() >= 1) {
            currentLevel = i;
            return;
        }
    }
}

namespace TaskIdle
{
    void Proc(void*, Message*)
    {
        while (!0) {
            io_hlt();
        }
    }
}

namespace TaskTest
{
    struct Valiables
    {
        int inputXPos;
        char s[32];

        Sheet* sht;
        Image* img;

        Timer* tim;
    };

    void Init(void* obj)
    {
        Valiables* o = (Valiables*)obj;

        o->inputXPos = 0;

        Task* task = taskman->GetCurrentTask();

        task->InitQueue(64);

        o->sht = shtman->Alloc();
        o->img = new Image(8 * 30, 25 + 16 * 2);
        Window::Make(o->img, "TaskTestFunc()");
        o->img->DrawLine(ConvRGB16(0x80, 0x80, 0x80), Point(6, 25 + 6), Point(232, 25 + 6));
        o->img->DrawLine(ConvRGB16(0x80, 0x80, 0x80), Point(6, 25 + 6), Point(6, 25 + 25));
        o->img->DrawLine(ConvRGB16(0xff, 0xff, 0xff), Point(7, 25 + 25), Point(233, 25 + 25));
        o->img->DrawLine(ConvRGB16(0xff, 0xff, 0xff), Point(233, 25 + 6), Point(233, 25 + 25));
        o->img->DrawRectangleFill(ConvRGB16(0xf0, 0xf0, 0xf0), Point(7, 25 + 7), Point(232, 25 + 24));
        o->sht->Init(o->img, Sheet::NoInvColor, Point(100, 100));
        o->sht->SetParentTask(task);
        shtman->SetHeight(o->sht, 2, Sheet::Normal);

        o->tim = timman->Alloc(task, 1);
        o->tim->SetTime(100);
    }


    void Proc(void* obj, Message* msg)
    {
        Valiables* o = (Valiables*)obj;

        if (msg->from == Message::From::Timer) {
            if (msg->arg == 1) {
                o->tim->SetTimePrev(100);
            }
        } else if (msg->from == Message::From::Keyboard) {
            o->s[0] = msg->arg;
            if (o->s[0] == '\b') {
                if (o->inputXPos > 0) {
                    o->inputXPos--;
                    o->sht->DrawRectangleFill(
                            ConvRGB16(0xf0, 0xf0, 0xf0),
                            Point(8 + 8 * o->inputXPos, 25 + 8),
                            Point(8 + 8 + 8 * o->inputXPos - 1, 25 + 8 + 16 - 1)
                            );
                }
            } else if (o->inputXPos < 28) {
                o->s[1] = '\0';
                o->sht->DrawString(
                        ConvRGB16(0x0000ff), o->s, Point(8 + 8 * o->inputXPos, 25 + 8), fontHankaku);
                o->inputXPos++;
            }
        }
    }

}

void DummyFunc(void*)
{
}

void DummyFunc(void*, Message*)
{
}

TaskManager* taskman;
