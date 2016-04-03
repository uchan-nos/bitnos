/*
 * specialtask.h
 *
 *  Created on: 2009/08/16
 *      Author: uchan
 */

#ifndef SPECIALTASK_H_
#define SPECIALTASK_H_

#include <bitnos/mtask.h>

class SpecialTask {
    static const int MaxTasks = 3;
    static Task* tasks[MaxTasks];
    public:
    static const int Keyboard = 0; // キーボードタスク
    static const int Mouse    = 1; // マウスタスク
    static const int Active   = 2; // アクティブなタスク

    static Task* Get(int i);
    static void Set(int i, Task* task);
};


#endif /* SPECIALTASK_H_ */
