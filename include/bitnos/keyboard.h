/*
 * keyboard.h
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
#include <bitnos/queue.h>
#include <bitnos/message.h>

class Task;

extern "C"
{
  void inthandler21(int* esp);
}

class Keyboard : private Uncopyable
{
  //static Queue<Message>* queue;
  static Task* handleTask;

public:
  //static void Init(Queue<Message>* queue);
  static void Init(Task* handleTask);

  static void IntHandler();
};

void TaskKeyboardFunc();
namespace TaskKeyboard
{
void Init(void* obj);
void PreProc(void* obj);
void Proc(void* obj, Message* msg);
}


#endif /* KEYBOARD_H_ */
