/*
 * specialtask.cpp
 *
 *  Created on: 2009/08/16
 *      Author: uchan
 */

#include <bitnos/specialtask.h>

Task* SpecialTask::tasks[MaxTasks];

Task* SpecialTask::Get(int i)
{
    if (0 <= i && i < MaxTasks) {
        return tasks[i];
    }
    return 0;
}

void SpecialTask::Set(int i, Task* task)
{
    if (0 <= i && i < MaxTasks) {
        tasks[i] = task;
    }
}
