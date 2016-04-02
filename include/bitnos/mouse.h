/*
 * mouse.h
 *
 *  Created on: 2009/08/01
 *      Author: uchan
 */

#ifndef MOUSE_H_
#define MOUSE_H_

#include <stdint.h>
#include <bitnos/queue.h>
#include <bitnos/message.h>

class Task;

extern "C"
{
  void inthandler2c(int* esp);
}

class Mouse : private Uncopyable
{
  static const uint8_t CmdEnable = 0xf4;

  //static Queue<Message>* queue;
  static Task* handleTask; // メッセージを送るタスク

  enum Type {
    Normal3, // 普通の3ボタンマウス
    Wheel3, // スクロールホイールマウス
    Wheel5 // スクロールホイール+5ボタンマウス
  };

  static Type mtype;

  struct Command
  {
    static const uint8_t EnableMouse = 0xf4;
  };

  /*
   * マウスから受信したデータを扱いやすくするデータ構造
   */
#pragma pack(push, 1)
  union RawData {
    uint8_t Byte[4];
    struct {
      union {
        uint8_t Byte;
        struct {
          unsigned Left: 1;
          unsigned Right: 1;
          unsigned Middle: 1;
          unsigned : 1; // Always 1
          unsigned XSign: 1;
          unsigned YSign: 1;
          unsigned XOverflow: 1;
          unsigned YOverflow: 1;
        } Bit;
      } Button;
      uint8_t XMove;
      uint8_t YMove;
      union {
        uint8_t Byte;
        struct {
          unsigned ZMove: 3;
          unsigned ZSign: 1;
          unsigned Button4th: 1;
          unsigned Button5th: 1;
          unsigned: 2; // Always 0
        } Bit;
      } Ext;
    } Detail;
  };
#pragma pack(pop)
  static RawData buf;

public:
  /*
   * 整形されたデータを表す
   */
  struct Data {
    bool ButtonMiddle;
    bool ButtonRight;
    bool ButtonLeft;
    bool Button5th;
    bool Button4th;
    int XMove;
    int YMove;
    int ZMove;
  };

private:
  static Data currentData;

public:
  //static void Init(Queue<Message>* queue);
  static void Init(Task* handleTask);

  static void IntHandler();

  /*
   * 受信データを解析する
   * 完了したらtrueを返す
   */
  static bool ReceivedData(uint8_t data);

  /*
   * 解析済みデータを受け取る
   */
  static const Data* GetData()
  {
    return &currentData;
  }

};

#define MOUSE_DEFAULT_IMAGE_XSIZE 10
#define MOUSE_DEFAULT_IMAGE_YSIZE 17
extern uint16_t mouseDefaultImage[];

void TaskMouseFunc();
namespace TaskMouse
{
void Init(void* obj);
void PreProc(void* obj);
void Proc(void* obj, Message* msg);
}

#endif /* MOUSE_H_ */
