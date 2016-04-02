/*
 * timer.h
 *
 *  Created on: 2009/08/03
 *      Author: uchan
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>

class Task;

class TimerManager;
extern TimerManager* timman;

class Timer
{
  friend class TimerManager;

  // TimerManagerの管理用情報
  Timer* next;

  enum {
    Invalid, Inactive, Active
  } flag;

  uint32_t time;

  Task* task;

  uint32_t data;

public:
  /*
   * タイムアウト時刻を設定する
   * time : 現在時刻からタイムアウトまでの時間
   */
  void SetTime(uint32_t time);

  /*
   * タイムアウト時刻を設定する
   * １回目の設定なら現在時刻を基点とする。
   * time : 前回設定時からタイムアウトまでの時間
   */
  void SetTimePrev(uint32_t time);

  /*
   * 送信するデータを設定する
   */
  void SetData(uint32_t data);
};

/*
 * タスクスイッチ用タイマー
 */
extern Timer* taskTimer;


class TimerManager : private Uncopyable
{
private:
  static const int MaxTimers = 512;

  static const uint32_t Infinite = (uint32_t)-1;

private:
  Timer timers0[MaxTimers];

  // 未使用タイマーリストの先頭
  Timer* timInvalidBegin;

  // アクティブでないタイマーのリストの先頭
  Timer* timInactiveBegin;

  // アクティブなタイマーのリストの先頭（番兵あり）
  // 時間順にソートされている
  Timer* timActiveBegin;

  // 現在の時刻
  // たまに初期化される
  uint32_t currentTime;

public:
  /*
   * コンストラクタ
   */
  TimerManager();

  /*
   * タイマーの割り当て
   */
  Timer* Alloc(Task* task, uint32_t data);

  /*
   * タイマーの開放
   */
  void Free(Timer* tim);

  /*
   * タイムアウト時刻を設定する
   * time : 現在時刻からタイムアウトまでの時間
   */
  void SetTime(Timer* tim, uint32_t time);

  /*
   * タイムアウト時刻を設定する
   * １回目の設定なら現在時刻を基点とする。
   * time : 前回設定時からタイムアウトまでの時間
   */
  void SetTimePrev(Timer* tim, uint32_t time);

  /*
   * 割り込みハンドラ
   */
  void IntHandler();

private:
  /*
   * 初期化関数
   * コンストラクタの肥大化を避ける
   */
  void Init();

  /*
   * 未使用リストに追加する
   */
  void TimerFree(Timer* tim);

  /*
   * 検索してリストからはずす（はずせるのは２番目以降の要素）
   * start : 検索開始場所
   */
  void SearchDetach(Timer* start, Timer* tim);

  /*
   * 指定されたタイマがActiveまたはInactiveならリストからはずす
   * 内部でSearchDetachを利用する
   */
  void Detach(Timer* tim);

  /*
   * アクティブリストに追加する
   */
  void Activate(Timer* tim);

  /*
   * 非アクティブリストに追加する
   */
  void Inactivate(Timer* tim);

  /*
   * タイムアウト時刻を設定する
   * time : タイムアウトの時刻（絶対時刻）
   */
  void SetTime0(Timer* tim, uint32_t time);
};

#endif /* TIMER_H_ */
