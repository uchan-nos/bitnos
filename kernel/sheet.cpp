/*
 * sheet.cpp
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#include <string.h>
#include <bitnos/sheet.h>
#include <bitnos/debug.h>
#include <bitnos/funcs.h>
#include <bitnos/mtask.h>
#include <bitnos/int.h>

void Sheet::Init(Image* image, uint32_t invColor, Point pos)
{
    this->image = image;
    this->invColor = invColor;
    this->pos = pos;
    this->autoRefresh = true;
    this->parentTask = 0;
}

void Sheet::Slide(Point pos)
{
    if (!IsFixed()) {
        Point old = this->pos;
        Point size(image->GetWidth() - 1, image->GetHeight() - 1);
        this->pos = pos;
        shtman->Refresh(0, old, old + size);
        shtman->Refresh(this);
    }
}

void Sheet::AutoRefresh(bool flag)
{
    autoRefresh = flag;
}

void Sheet::Refresh()
{
    shtman->Refresh(this);
}

bool Sheet::IsFixed()
{
    if (type == Background) {
        return true;
    } else {
        return false;
    }
}

void Sheet::DrawPoint(uint16_t col, Point pos)
{
    image->DrawPoint(col, pos);
    if (autoRefresh) {
        shtman->Refresh(this, pos, pos);
    }
}

void Sheet::DrawRectangle(uint16_t col, Point pos0, Point pos1)
{
    Point dx(pos1.X - pos0.X, 0);
    Point dy(0, pos1.Y - pos0.Y);
    image->DrawRectangle(col, pos0, pos1);
    if (autoRefresh) {
        shtman->Refresh(this, pos0, pos0 + dx);
        shtman->Refresh(this, pos0 + dx, pos1);
        shtman->Refresh(this, pos0, pos0 + dy);
        shtman->Refresh(this, pos0 + dy, pos1);
    }
}

void Sheet::DrawRectangleFill(uint16_t col, Point pos0, Point pos1)
{
    image->DrawRectangleFill(col, pos0, pos1);
    if (autoRefresh) {
        shtman->Refresh(this, pos0, pos1);
    }
}

void Sheet::DrawLine(uint16_t col, Point pos0, Point pos1)
{
    image->DrawLine(col, pos0, pos1);
    if (autoRefresh) {
        shtman->Refresh(this, pos0, pos1);
    }
}
void Sheet::DrawChar(uint16_t col, char ch, Point pos, BitmapFont* font)
{
    image->DrawChar(col, ch, pos, font);
    if (autoRefresh) {
        Point pos1(pos);
        pos1.X += font->GetWidth() - 1;
        pos1.Y += font->GetHeight() - 1;
        shtman->Refresh(this, pos, pos1);
    }
}

void Sheet::DrawString(uint16_t col, const char* str, Point pos, BitmapFont* font)
{
    image->DrawString(col, str, pos, font);
    if (autoRefresh) {
        Point pos1(pos);
        pos1.X += font->GetWidth() * strlen(str) - 1;
        pos1.Y += font->GetHeight() - 1;
        shtman->Refresh(this, pos, pos1);
    }
}


SheetManager* shtman;

SheetManager::SheetManager(Image* screen) :
    screen(screen), map(screen->GetWidth(), screen->GetHeight())
{
    int i;

    top = -1;

    topMostBottom = mouseBottom = normalBottom = 0;

    for (i = 0; i < MaxSheets; i++) {
        sheets0[i].status = Sheet::Free;
    }
}

Sheet* SheetManager::Alloc()
{
    int i;
    Sheet* sht = 0;
    for (i = 0; i < MaxSheets; i++) {
        if (sheets0[i].status == Sheet::Free) {
            sht = sheets0 + i;
            sht->status = Sheet::Using;
            sht->height = -1;
            break;
        }
    }
    return sht;
}

void SheetManager::Free(Sheet* sht)
{
    if (sht != 0) {
        sht->status = Sheet::Free;
    }
}

void SheetManager::SetHeight(Sheet* sht, int height, Sheet::Type type)
{
    if (sht == 0) {
        return;
    }

    int i;
    int oldHeight = sht->height;

    if (height < -1) {
        height = -1;
    } else if (height > top + 1) {
        height = top + 1;
    }

    DINT
        if (oldHeight < height) {
            // 以前より高くなる
            if (oldHeight >= 0) {
                if (type == Sheet::Background && height >= normalBottom) {
                    height = normalBottom - 1;
                } else if (type == Sheet::Normal && height >= topMostBottom) {
                    height = topMostBottom - 1;
                } else if (type == Sheet::TopMost && height >= mouseBottom) {
                    height = mouseBottom - 1;
                }
                // 以前も表示中だった。中間の高さを1ずつ減らす
                for (i = oldHeight; i < height; i++) {
                    sheets[i] = sheets[i + 1];
                    sheets[i]->height = i;
                }
            } else {
                if (type == Sheet::Background) {
                    if (height > normalBottom) {
                        height = normalBottom;
                    }
                    normalBottom++;
                    topMostBottom++;
                    mouseBottom++;
                } else if (type == Sheet::Normal) {
                    if (height > topMostBottom) {
                        height = topMostBottom;
                    } else if (height < normalBottom) {
                        height = normalBottom;
                    }
                    topMostBottom++;
                    mouseBottom++;
                } else if (type == Sheet::TopMost) {
                    if (height > mouseBottom) {
                        height = mouseBottom;
                    } else if (height < topMostBottom) {
                        height = topMostBottom;
                    }
                    mouseBottom++;
                } else if (type == Sheet::Mouse) {
                    if (height > top + 1) {
                        height = top + 1;
                    } else if (height < mouseBottom) {
                        height = mouseBottom;
                    }
                }
                // 以前は非表示だった。上のシートの高さを1ずつ増やす
                for (i = top + 1; i > height; i--) {
                    sheets[i] = sheets[i - 1];
                    sheets[i]->height = i;
                }
                top++; // 最大の高さが増える
            }
        } else if (oldHeight > height) {
            // 以前より低くなる
            if (height >= 0) {
                if (type == Sheet::Normal && height < normalBottom) {
                    height = normalBottom;
                } else if (type == Sheet::TopMost && height < topMostBottom) {
                    height = topMostBottom;
                } else if (type == Sheet::Mouse && height < mouseBottom) {
                    height = mouseBottom;
                }
                // まだ表示中。中間の高さを1ずつ増やす
                for (i = oldHeight; i > height; i--) {
                    sheets[i] = sheets[i - 1];
                    sheets[i]->height = i;
                }
            } else {
                if (type == Sheet::Background) {
                    normalBottom--;
                    topMostBottom--;
                    mouseBottom--;
                } else if (type == Sheet::Normal) {
                    topMostBottom--;
                    mouseBottom--;
                } else if (type == Sheet::TopMost) {
                    mouseBottom--;
                }
                // あたらに非表示になる。上のシートの高さを1ずつ減らす
                for (i = oldHeight; i < top; i++) {
                    sheets[i] = sheets[i + 1];
                    sheets[i]->height = i;
                }
                top--; // 最大の高さが減る
            }
        }

    RINT

        sht->height = height;

    sht->type = type;

    // 目的の位置に格納
    sheets[height] = sht;
    // シートを描画しなおす
    Refresh(sht);
}

void SheetManager::Refresh(Sheet* sht)
{
    Refresh(sht, Point(0, 0), Point(sht->image->GetWidth() - 1, sht->image->GetHeight() - 1));
}

void SheetManager::Refresh(Sheet* sht, Point pos0, Point pos1)
{

    // 指定されたシートから上のシートを再描画する

    // シートが指定されない場合、一番下のシートから描画する
    if (sht == 0) {
        if (sheets[0] == 0) {
            return;
        }
        sht = sheets[0];
    }

    // 絶対座標に変換
    pos0 += sht->pos;
    pos1 += sht->pos;

    if (pos0.X < 0) {
        pos0.X = 0;
    }
    if (pos1.X >= screen->GetWidth()) {
        pos1.X = screen->GetWidth() - 1;
    }

    if (pos0.Y < 0) {
        pos0.Y = 0;
    }
    if (pos1.Y >= screen->GetHeight()) {
        pos1.Y = screen->GetHeight() - 1;
    }

    int maph;
    if (sht->invColor < Sheet::NoInvColor) {
        maph = 0;
    } else {
        maph = sht->height;
    }

    RefreshMap(maph, pos0, pos1);
    Point pos;

    Sheet* s;
    uint16_t col;

    /*
       int h;
       uint16_t sid;

       for (h = sht->height; h <= top; h++) {
       s = sheets[h];
       sid = s - sheets0;
       for (pos.Y = pos0.Y; pos.Y <= pos1.Y; pos.Y++) {
       for (pos.X = pos0.X; pos.X <= pos1.X; pos.X++) {
       if (map.GetPoint(pos) == sid) {
       col = s->image->GetPoint(pos - s->pos);
       screen->DrawPoint(col, pos);
       }
       }
       }
       }
       */


    for (pos.Y = pos0.Y; pos.Y <= pos1.Y; pos.Y++) {
        for (pos.X = pos0.X; pos.X <= pos1.X; pos.X++) {
            s = sheets0 + map.GetPoint(pos);
            col = s->image->GetPoint(pos - s->pos);
            screen->DrawPoint(col, pos);
        }
    }

}

Sheet* SheetManager::GetTopSheet(Point pos, Sheet::Type type)
{
    int h = top;
    Sheet* sht;
    switch (type) {
        case Sheet::Background:
            h = normalBottom - 1;
            break;
        case Sheet::Normal:
            h = topMostBottom - 1;
            break;
        case Sheet::TopMost:
            h = mouseBottom - 1;
            break;
        case Sheet::Mouse:
            h = top;
            break;
    }
    for (; h >= 0; h--) {
        sht = sheets[h];
        if (sht->pos.X <= pos.X && pos.X < sht->pos.X + sht->image->width &&
                sht->pos.Y <= pos.Y && pos.Y < sht->pos.Y + sht->image->height &&
                sht->image->GetPoint(pos - sht->pos) != sht->NoInvColor) {
            return sht;
        }
    }
    return 0;
}

void SheetManager::MoveToTop(Sheet* sht)
{
    int h = top;
    switch (sht->type) {
        case Sheet::Background:
            h = normalBottom - 1;
            break;
        case Sheet::Normal:
            h = topMostBottom - 1;
            break;
        case Sheet::TopMost:
            h = mouseBottom - 1;
            break;
        case Sheet::Mouse:
            h = top;
            break;
    }
    SetHeight(sht, h, sht->type);
}

void SheetManager::RefreshMap(int h0, Point pos0, Point pos1)
{
    int h;
    Point pos;
    uint16_t sid; // シートID

    Sheet* s;

    Point shtpos0, shtpos1; // シートに関する相対座標

    // マップへ書き込む
    for (h = h0; h <= top; h++) {
        s = sheets[h];
        sid = s - sheets0; // シートIDの計算

        // 描画範囲の計算
        shtpos0 = pos0 - s->pos;
        shtpos1 = pos1 - s->pos;
        if (shtpos0.Y < 0) {
            shtpos0.Y = 0;
        }
        if (shtpos1.Y >= s->image->GetHeight()) {
            shtpos1.Y = s->image->GetHeight() - 1;
        }
        if (shtpos0.X < 0) {
            shtpos0.X = 0;
        }
        if (shtpos1.X >= s->image->GetWidth()) {
            shtpos1.X = s->image->GetWidth() - 1;
        }
        if (s->invColor >= Sheet::NoInvColor) {
            // 透明色は無効になっているから、高速化できる
            /*
               shtpos0 += s->pos;
               shtpos1 += s->pos;
               map.DrawRectangleFill(sid, shtpos0, shtpos1);
               */

            /*
               for (pos.Y = shtpos0.Y; pos.Y <= shtpos1.Y; pos.Y++) {
               for (pos.X = shtpos0.X; pos.X <= shtpos1.X; pos.X++) {
               map.DrawPoint(sid, pos + s->pos);
               }
               }
            //*/
            /*
               for (pos.Y = shtpos0.Y + s->pos.Y; pos.Y <= shtpos1.Y + s->pos.Y; pos.Y++) {
               for (pos.X = shtpos0.X + s->pos.X; pos.X <= shtpos1.X + s->pos.X; pos.X++) {
               map.DrawPoint(sid, pos);
               }
               }
            //*/
            if ((shtpos0.X <= shtpos1.X) && (shtpos0.Y <= shtpos1.Y)) {
                map.DrawRectangleFill(sid, shtpos0 + s->pos, shtpos1 + s->pos);
            }


        } else {
            // 透明色は無効になっていない
            for (pos.Y = shtpos0.Y; pos.Y <= shtpos1.Y; pos.Y++) {
                for (pos.X = shtpos0.X; pos.X <= shtpos1.X; pos.X++) {
                    if (s->invColor != s->image->GetPoint(pos)) {
                        // 透明色じゃなければ書き込む
                        map.DrawPoint(sid, pos + s->pos);
                    }
                }
            }
        }
    }
}
