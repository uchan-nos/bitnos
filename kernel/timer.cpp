/*
 * timer.cpp
 *
 *  Created on: 2009/08/03
 *      Author: uchan
 */

#include <bitnos/timer.h>
#include <bitnos/funcs.h>
#include <bitnos/mtask.h>
#include <bitnos/debug.h>
#include <bitnos/int.h>
#include <bitnos/message.h>

TimerManager* timman;
Timer* taskTimer;

class TaskManager;

void Timer::SetTime(uint32_t time)
{
    timman->SetTime(this, time);
}

void Timer::SetTimePrev(uint32_t time)
{
    timman->SetTimePrev(this, time);
}

void Timer::SetData(uint32_t data)
{
    this->data = data;
}


TimerManager::TimerManager()
{
    Init();
}

Timer* TimerManager::Alloc(Task* task, uint32_t data)
{
    Timer* tim = timInvalidBegin;
    if (tim != 0) {
        // 未使用リストから非アクティブリストへ移動させる
        timInvalidBegin = tim->next;
        tim->next = timInactiveBegin;
        timInactiveBegin = tim;

        tim->flag = Timer::Inactive;
        tim->time = Infinite;
        tim->task = task;
        tim->data = data;
    }
    return tim;
}

void TimerManager::Free(Timer* tim)
{
    if (tim != 0) {
        // リストを操作しているときに割り込みが起こるとまずい
        DINT

            // 未使用リストに追加する
            Detach(tim);
        TimerFree(tim);

        RINT
    }
}

void TimerManager::SetTime(Timer* tim, uint32_t time)
{
    SetTime0(tim, currentTime + time);
}

void TimerManager::SetTimePrev(Timer* tim, uint32_t time)
{
    if (tim != 0 && tim->time != Infinite) {
        // 前回設定時刻を基点とする
        SetTime0(tim, tim->time + time);
    } else if (tim != 0 && tim->time == Infinite) {
        // 初回設定時は現在時刻を基点とする
        SetTime0(tim, currentTime + time);
    }
}

void TimerManager::IntHandler()
{
    bool taskSwitch = false;
    currentTime++;

    if (timActiveBegin->time > currentTime) {
        return;
    }
    //Debug::WriteLine("timer inthnd found timeouted timer");
    Timer* prev = timActiveBegin;
    Timer* next = 0;
    while (prev->time <= currentTime) {
        // タイマーprevがタイムアップした
        timActiveBegin = prev->next;
        next = prev->next;

        // 非アクティブリストに追加
        Inactivate(prev);

        if (prev == taskTimer) {
            // タスク切り替えを後でする
            taskSwitch = true;
        } else if (prev->task != 0) {
            taskman->SendMessage(
                    prev->task,
                    Message(Message::From::Timer, prev->data)
                    );
        }
        prev = next;
    }

    if (taskSwitch) {
        taskman->Switch();
    }
}

void TimerManager::Init()
{
    timInvalidBegin = timers0 + 1;
    timInactiveBegin = 0;
    timActiveBegin = timers0;
    currentTime = 0;

    int i;
    for (i = 1; i < MaxTimers - 1; i++) {
        timers0[i].flag = Timer::Invalid;
        timers0[i].next = timers0 + i + 1;
    }
    timers0[i].flag = Timer::Invalid;
    timers0[i].next = 0;

    // 番兵の追加
    timers0[0].time = Infinite;
    timers0[0].flag = Timer::Active;
    timers0[0].next = 0;
    timers0[0].task = 0;
}

void TimerManager::TimerFree(Timer* tim)
{
    // 未使用リストに追加する
    tim->next = timInvalidBegin;
    timInvalidBegin = tim;
    tim->flag = Timer::Invalid;
}

void TimerManager::SearchDetach(Timer* start, Timer* tim)
{
    while (start != 0) {
        if (start->next == tim) {
            start->next = tim->next;
            break;
        }
        start = start->next;
    }
}

void TimerManager::Detach(Timer* tim)
{
    if (tim->flag == Timer::Active) {
        // タイマーがアクティブなときに開放する
        if (timActiveBegin == tim) {
            // 先頭
            timActiveBegin = tim->next;
        } else {
            // 先頭ではない
            SearchDetach(timActiveBegin, tim);
        }
    } else if (tim->flag == Timer::Inactive) {
        // アクティブでないときに開放する
        if (timInactiveBegin == tim) {
            // 先頭
            timInactiveBegin = tim->next;
        } else {
            // 先頭ではない
            SearchDetach(timInactiveBegin, tim);
        }
    }
}

void TimerManager::Activate(Timer* tim)
{
    DINT
        if (tim != 0 && tim->flag == Timer::Inactive && currentTime <= tim->time) {
            tim->flag = Timer::Active;

            if (tim->time <= timActiveBegin->time) {
                tim->next = timActiveBegin;
                timActiveBegin = tim;
            } else {
                Timer* prev = timActiveBegin;
                while (!0) {
                    if (tim->time <= prev->next->time) {
                        tim->next = prev->next;
                        prev->next = tim;
                        break;
                    }
                    prev = prev->next;
                }
            }
        }
    RINT
}

void TimerManager::Inactivate(Timer* tim)
{
    DINT
        if (tim != 0 && tim->flag == Timer::Active) {
            Detach(tim);
            tim->flag = Timer::Inactive;
            tim->next = timInactiveBegin;
            timInactiveBegin = tim;
        }
    RINT
}

void TimerManager::SetTime0(Timer* tim, uint32_t time)
{
    DINT
        if (tim != 0 && tim->flag != Timer::Invalid) {
            // とりあえずリストからはずしておく
            Detach(tim);
            tim->flag = Timer::Inactive;

            tim->time = time;
            Activate(tim);
        }
    RINT
}

