/*
 * sheet.h
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#ifndef SHEET_H_
#define SHEET_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>
#include <bitnos/point.h>
#include <bitnos/IDraw.h>
#include <bitnos/image.h>

class SheetManager;
extern SheetManager* shtman;

class Task;

/*
 * シートを表す構造体
 */
class Sheet : public IDraw
{
  friend class SheetManager;

public:
  enum Type {
    Normal, // 普通の高さの扱い
    TopMost, // 最前面に表示する。これより上にはTopMostかMouse以外のシートを表示させない
    Mouse, // マウスのシート。これより上にはMouse以外のシートを表示させない
    Background // 背景のシート。これより下には何も表示させない
  };

  Sheet()
  {}

  virtual ~Sheet()
  {}

  static const uint32_t NoInvColor = 0x10000;

  Image* image; // 描画データへのポインタ
  uint32_t invColor; // 透明色
  Point pos;

  /*
   * 初期化する
   * image : 画像本体
   * invColor : 透明色
   * pos : 初期位置
   */
  void Init(Image* image, uint32_t invColor, Point pos);

  /*
   * シートをXY方向に移動させる
   * pos : 現在座標に関する相対座標
   */
  void Slide(Point pos);

  /*
   * 自動リフレッシュの設定
   * flag : 自動リフレッシュする = true
   */
  void AutoRefresh(bool flag);

  /*
   * リフレッシュする
   */
  void Refresh();

  /*
   * シートがスライド禁止かどうか
   */
  bool IsFixed();

  /*
   * シートが属しているタスクを取得
   */
  Task* GetParentTask()
  {
    return parentTask;
  }

  /*
   * シートが属しているタスクを設定
   */
  void SetParentTask(Task* task)
  {
    parentTask = task;
  }

public:
  // IDrawの実装
  virtual void DrawPoint(uint16_t col, Point pos);

  virtual void DrawRectangle(uint16_t col, Point pos0, Point pos1);
  virtual void DrawRectangleFill(uint16_t col, Point pos0, Point pos1);

  virtual void DrawLine(uint16_t col, Point pos0, Point pos1);

  virtual void DrawChar(uint16_t col, char ch, Point pos, BitmapFont* font);

  virtual void DrawString(uint16_t col, const char* str, Point pos, BitmapFont* font);

private:
  bool autoRefresh;

  /*
   * このシートが属しているタスク
   */
  Task* parentTask;
private:
  /*
   * 以下のprivateなメンバは、SheetManagerクラスが利用する管理情報
   * 他のコードからは利用してはいけない
   */

  /*
   * シートの状態
   */
  enum Status {
    Using, Free
  } status;

  /*
   * シートの性質
   */
  Type type;

  int height; // シートの高さ
};
/*
struct WindowFrameSheet : public Sheet
{
protected:
public:
};
*/
class SheetManager : private Uncopyable
{
private:
  static const int MaxSheets = 512;

private:
  Image* screen; // 描画する場所
  Image map; // シートのマップデータ
  Sheet sheets0[MaxSheets]; // シート構造体本体
  Sheet* sheets[MaxSheets]; // シートの高さ順に並ぶ
  int top; // 一番上のシートの高さ
  int topMostBottom; // 最前面シートの高さの最低値
  int mouseBottom; // マウスシートの高さの最低値
  int normalBottom; // 通常シートの高さの最低値

public:
  /*
   * コンストラクタ
   * screen : 描画する場所
   */
  SheetManager(Image* screen);

  /*
   * シート確保
   */
  Sheet* Alloc();

  /*
   * シート開放
   */
  void Free(Sheet* sht);

  /*
   * シートの高さを設定する
   * 他のシートの高さをそれにあわせて変更する
   */
  void SetHeight(Sheet* sht, int height, Sheet::Type type = Sheet::Normal);

  /*
   * シートを再描画する
   */
  void Refresh(Sheet* sht);

  /*
   * シートを再描画する
   * pos0, pos1 : 再描画する範囲。シートの中での相対座標
   */
  void Refresh(Sheet* sht, Point pos0, Point pos1);

  /*
   * 指定位置の一番上のシートを取得する
   * type : 検索するシートのタイプ
   */
  Sheet* GetTopSheet(Point pos, Sheet::Type type);

  /*
   * シートを指定のタイプの中で一番上にする
   */
  void MoveToTop(Sheet* sht);

private:
  /*
   * マップを再構築する
   * h0 : シートの高さ。このシートとここより上のシートを対象にする
   * pos0, pos1 : 絶対座標
   */
  void RefreshMap(int h0, Point pos0, Point pos1);
};


#endif /* SHEET_H_ */
