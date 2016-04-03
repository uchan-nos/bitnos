/*
 * TaskFuncs.h
 *
 *  Created on: 2009/10/14
 *      Author: uchan
 */

#ifndef TASKFUNCS_H_
#define TASKFUNCS_H_

class Message;
struct TSS32;

#include <stdint.h>

typedef void (*TaskFuncInit)(void* obj);
typedef void (*TaskFuncPreProc)(void* obj);
typedef void (*TaskFuncProc)(void* obj, Message* msg);

/*
 * obj : タスクで使う変数領域の先頭アドレス
 * init : タスクの初期化関数
 * proc : タスクのメイン処理関数
 */
void GeneralFunction(void* obj, TaskFuncInit init, TaskFuncPreProc preproc, TaskFuncProc proc);

void SetTaskFuncs(
        TSS32* tss, uintptr_t stackTop, uintptr_t stackSize,
        TaskFuncInit init, TaskFuncPreProc preproc, TaskFuncProc proc);

#endif /* TASKFUNCS_H_ */
