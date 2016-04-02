/*
 * mtask.h
 *
 *  Created on: 2009/08/12
 *      Author: uchan
 */

#ifndef MTASK_H_
#define MTASK_H_

#include <bitnos/tss.h>
#include <bitnos/queue.h>
#include <bitnos/queue.h>
#include <bitnos/message.h>
#include <bitnos/desctable.h>
#include <bitnos/timer.h>
#include <bitnos/funcs.h>

#include <bitnos/TaskFuncs.h>


class TaskManager;
extern TaskManager* taskman;

class Task
{
  friend class TaskManager;

  TSS32 tss; // タスク状態退避領域
  int selector; // セレクタ（GDT番号）
  Queue<Message>* queue; // タスク用FIFOバッファ
  SegmentDescriptor ldt[2]; // ローカルディスクリプタテーブル

  int time; // タスクタイマの設定値（優先度）
  int level; // タスクの動作レベル

  enum {
    Invalid, Inactive, Active
  } flag;

  Task* next; // 線形リスト用

public:

  void InitQueue(int size);

  void InitQueue(Queue<Message>* queue);

  void DisposeQueue();

  Queue<Message>* GetQueue()
  {
    return queue;
  }

  void SetEipEsp(uint32_t eip, uint32_t esp)
  {
    tss.eip = eip;
    tss.esp = esp;
  }

  void SetFuncs(
      uintptr_t stackTop, uintptr_t stackSize,
      TaskFuncInit init, TaskFuncPreProc preproc, TaskFuncProc proc)
  {
    SetTaskFuncs(&tss, stackTop, stackSize, init, preproc, proc);
  }

};

class TaskManager
{
private:
  static const int MaxTasks = 1024; // タスクの最大数
  static const int MaxLevels = 16; // レベルの数
  static const int MaxLevelTasks = 256; // 1つのレベルに入るタスクの最大数
  static const uint32_t TSSBegin = 1024; // TSSを割り当てるGDT番号

private:
  Task tasks0[MaxTasks]; // タスク構造体本体
  Queue<Task*>* taskLevel[MaxLevels];
  Task* taskInvalidBegin;

  int currentLevel;
  bool changeLevel;

public:
  /*
   * コンストラクタ
   */
  TaskManager();

  /*
   * タスクスケジューリングの開始
   */
  void StartScheduling();

  /*
   * タスク割り当て
   */
  Task* Alloc();

  /*
   * タスク解放
   */
  void Free(Task* task);

  /*
   * タスク切り替え
   */
  void Switch();

  /*
   * タスクを動作リストに追加
   */
  void Run(Task* task, int time, int level);

  /*
   * タスクを動作リストから削除
   */
  void Sleep(Task* task);

  /*
   * タスクにメッセージを送る
   */
  void SendMessage(Task* task, const Message& msg);

  /*
   * 現在実行中のタスクを取得する
   */
  Task* GetCurrentTask();

private:
  /*
   * 一番上のレベルを探してcurrentLevelにセットする
   */
  void SearchCurrentLevel();
};

void TaskIdleFunc();
void TaskTestFunc();

namespace TaskIdle
{
void Proc(void*, Message*);
}

namespace TaskTest
{
void Init(void* obj);
void Proc(void* obj, Message* msg);
}

void DummyFunc(void*);
void DummyFunc(void*, Message*);


#endif /* MTASK_H_ */
