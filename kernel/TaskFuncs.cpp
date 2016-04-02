/*
 * TaskFuncs.cpp
 *
 *  Created on: 2009/10/05
 *      Author: uchan
 */

#include <bitnos/TaskFuncs.h>
#include <bitnos/mtask.h>
#include <bitnos/queue.h>
#include <bitnos/message.h>
#include <bitnos/debug.h>

void GeneralFunction(void* obj, TaskFuncInit init, TaskFuncPreProc preproc, TaskFuncProc proc)
{
    init(obj);
    //Debug::WriteLine("obj=%08x", (uint32_t)obj);

    Task* task = taskman->GetCurrentTask();
    Queue<Message>* queue = task->GetQueue();
    Message* msg;

    while (!0) {
        preproc(obj);
        io_cli();
        if (queue->GetCount() == 0) {
            taskman->Sleep(task);
            io_sti();
        } else {
            io_sti();
            msg = &queue->Front();

            proc(obj, msg);

            io_cli();
            queue->PopFront();
            io_sti();
        }
    }
}

void EndTaskFunction()
{
    Debug::WriteLine("task returened.");
    while (!0);
}

void SetTaskFuncs(
        TSS32* tss, uintptr_t stackTop, uintptr_t stackSize,
        TaskFuncInit init, TaskFuncPreProc preproc, TaskFuncProc proc)
{
    tss->eip = (uint32_t)GeneralFunction;
    tss->esp = stackTop + stackSize - 5 * sizeof(uint32_t);
    uint32_t* stack = (uint32_t*)tss->esp;
    stack[0] = (uint32_t)EndTaskFunction;
    stack[1] = stackTop;
    stack[2] = (uint32_t)init;
    stack[3] = (uint32_t)preproc;
    stack[4] = (uint32_t)proc;
}


