/*
 * memory.cpp
 *
 *  Created on: 2009/07/08
 *      Author: uchan
 */

#include <string.h>
#include <bitnos/memory.h>
#include <bitnos/funcs.h>
#include <bitnos/int.h>

// アライメントチェックフラグ
#define EFLAGS_ACFLAG 0x00040000
#define CACHE_DISABLE 0x60000000

uint32_t GetPhysicalMemoryCapacitySub(uintptr_t begin, uintptr_t end, int increment);

uint32_t GetPhysicalMemoryCapacity(uintptr_t begin, uintptr_t end)
{
    uint32_t size = 0;
    uint32_t eflags, cr0;
    bool i486 = false;

    // CPUの種類(386, 486以降)を判断する
    eflags = io_load_eflags();
    eflags |= EFLAGS_ACFLAG;
    io_store_eflags(eflags);
    eflags = io_load_eflags();

    // 386は、AC=1にしても自動でクリアされる
    if ((eflags & ~EFLAGS_ACFLAG) != 0) {
        i486 = true;
    }

    eflags &= ~EFLAGS_ACFLAG; // AC = 0
    io_store_eflags(eflags);

    if (i486) {
        // CPUのキャッシュをOFFにする
        cr0 = load_cr0();
        cr0 |= CACHE_DISABLE;
        store_cr0(cr0);
    }
    size = GetPhysicalMemoryCapacitySub(begin, end, 1024 * 1024);
    if (i486) {
        // CPUのキャッシュをONにする
        cr0 = load_cr0();
        cr0 &= ~CACHE_DISABLE;
        store_cr0(cr0);
    }

    return begin + size;
}

/*
 * incrementずつメモリを調べ、endを超えない最大のサイズを返す
 */
uint32_t GetPhysicalMemoryCapacitySub(uintptr_t begin, uintptr_t end, int increment)
{
    volatile uint32_t* p = (uint32_t*)begin;
    uint32_t temp = 0;
    uint32_t size = 0;

    const uint32_t pat0 = 0xa55a5aa5;


    if (increment == 0) {
        increment = 1;
    }

    while ((uintptr_t)p < end) {
        temp = *p;
        *p = pat0;
        if (*p != pat0) {
            goto end_check;
        }
        *p = ~pat0;
        if (*p != ~pat0) {
            goto end_check;
        }
        *p = temp;
        if (*p != temp) {
            goto end_check;
        }
        if (((uintptr_t)p + increment) < end) {
            p = (uint32_t*)((uintptr_t)p + increment);
            size += increment;
        }
    }

end_check:
    *p = temp;
    if (increment == 1) {
        return size;
    } else {
        return size + GetPhysicalMemoryCapacitySub(begin + size, end, increment >> 10);
    }

}

#if MEMMAN == 1

// メモリマネージャの初期化
void MemoryManager::Init(uintptr_t start, uintptr_t end, MemInfo* memInfoBuf)
{
    mi0 = memInfoBuf;

    start = (start + Alignment) & ~(Alignment);
    end = end & ~(Alignment);

    int i;
    for (i = 1; i < MemInfoSize - 1; i++) {
        mi0[i].next = &mi0[i + 1];
    }
    mi0[i].next = 0;

    mi0[0].addr = start;
    mi0[0].size = end - start;
    mi0[0].next = 0;

    miInvalidBegin = &mi0[1];
    miFreeBegin = &mi0[0];
    miAllocBegin = 0;
}

// 割り当て
void* MemoryManager::Alloc(size_t size)
{
    size = (size + Alignment) & ~Alignment;
    MemInfo* mi = FreeListSearch(size);
    if (mi != 0) {
        AllocListAdd(mi);
        return (void*)mi->addr;
    }
    return 0;
}

// 開放
void MemoryManager::Free(void* ptr)
{
    FreeListAdd(
            AllocListRemove((uintptr_t)ptr));
}


MemInfo* MemoryManager::MemInfoAlloc()
{
    MemInfo* mi = miInvalidBegin;
    if (mi != 0) {
        miInvalidBegin = mi->next;
    }
    return mi;
}

void MemoryManager::MemInfoFree(MemInfo* mi)
{
    if (mi != 0) {
        mi->next = miInvalidBegin;
        miInvalidBegin = mi;
    }
}

void MemoryManager::FreeListAdd(MemInfo* mi)
{
    MemInfo* miprev = 0;
    MemInfo* minext = miFreeBegin;
    while (minext != 0 && minext->addr <= mi->addr) {
        miprev = minext;
        minext = minext->next;
    }
    if (miprev == 0) {
        // 先頭に追加
        mi->next = miFreeBegin;
        miFreeBegin = mi;
    } else {
        // 中間、または末尾に追加
        mi->next = miprev->next;
        miprev->next = mi;
    }

    // 結合できるかを探す
    if (miprev != 0 && miprev->addr + miprev->size == mi->addr) {
        // 前と結合できる
        miprev->size += mi->size;
        miprev->next = mi->next;
        MemInfoFree(mi);

        mi = miprev;
    }
    if (minext != 0 && mi->addr + mi->size == minext->addr) {
        // 後ろと結合できる
        mi->size += minext->size;
        mi->next = minext->next;
        MemInfoFree(minext);
    }
}

MemInfo* MemoryManager::FreeListSearch(size_t size)
{
    // sizeはアライメント済みと仮定

    MemInfo* miprev = 0;
    MemInfo* minext = miFreeBegin;
    MemInfo* minew = 0;
    while (minext != 0 && minext->size < size) {
        miprev = minext;
        minext = minext->next;
    }
    if (minext != 0) {
        uint32_t remain = minext->size - size;
        if (remain > 0) {
            // メモリ管理情報を1つ増やす
            minew = MemInfoAlloc();
            if (minew != 0) {
                minew->addr = minext->addr;
                minew->size = size;
                minext->addr += size;
                minext->size = remain;
            } else {
                // メモリ管理情報が足りない
                if (miprev == 0) {
                    miFreeBegin = minext->next;
                } else {
                    miprev->next = minext->next;
                }
                minew = minext;
            }
        } else {
            // remain == 0
            if (miprev == 0) {
                miFreeBegin = minext->next;
            } else {
                miprev->next = minext->next;
            }
            minew = minext;
        }

    }
    return minew;
}

void MemoryManager::AllocListAdd(MemInfo* mi)
{
    mi->next = miAllocBegin;
    miAllocBegin = mi;
}

MemInfo* MemoryManager::AllocListRemove(uintptr_t addr)
{
    MemInfo* miprev = 0;
    MemInfo* minext = miAllocBegin;
    while (minext != 0 && addr != minext->addr) {
        miprev = minext;
        minext = minext->next;
    }
    if (minext != 0) {
        if (miprev == 0) {
            miAllocBegin = minext->next;
        } else {
            miprev->next = minext->next;
        }
    }
    return minext;
}

#elif MEMMAN == 2

void MemoryManager::Init(uintptr_t begin, uint32_t size)
{
    /*
     * Memory Model
     *
     *  -------  begin
     * |MemInfo| Free List Begin
     * |-------|
     * |MemInfo| Alloc List Begin
     * |-------|
     * |       | dead space for next MemInfo
     * |-------|
     * |MemInfo| Free List Second
     * |-------| 16-byte alignment
     * |       |
     * | free  |
     * | space |
     * |       |
     * |-------|
     * |MemInfo| Alloc List End
     * |-------|
     * |MemInfo| Free List End
     *  -------  begin + size
     */

    MemInfo* mi;
    memBegin = begin = Ceil(begin);
    memSize = size = Floor(size);

    /*
     * 与えられたメモリ空間が少なすぎることのチェックはしない。
    //if (size < ((sizeof(MemInfo) * 3 + Alignment) & ~Alignment) + sizeof(MemInfo) * 2) {
    if (size < Ceil(sizeof(MemInfo) * 3) + sizeof(MemInfo) * 2) {
    // too few free space
    return;
    }
    */

    mi = (MemInfo*)begin;

    // free list begin
    freeBegin = (MemInfo*)begin + 0;
    Link(0, freeBegin);
    Link(freeBegin, (MemInfo*)(begin + Ceil(sizeof(MemInfo) * 3) - sizeof(MemInfo)));
    freeBegin->size = 0;
    freeBegin->flag = MemInfo::None;

    // alloc list begin
    allocBegin = (MemInfo*)begin + 1;
    Link(0, allocBegin);
    Link(allocBegin, (MemInfo*)(begin + size - sizeof(MemInfo) * 2));
    allocBegin->size = 0;
    allocBegin->flag = MemInfo::None;

    // alloc list end
    Link(allocBegin->next, 0);
    allocBegin->next->size = 0;
    allocBegin->next->flag = MemInfo::None;

    // free list second
    // MemInfoの後ろを指定のアライメントに合わせる
    Link(freeBegin->next, (MemInfo*)(begin + size - sizeof(MemInfo)));
    freeBegin->next->size = size - Ceil(sizeof(MemInfo) * 3) - sizeof(MemInfo) * 2;
    freeBegin->next->flag = MemInfo::Free;

    // free list end
    Link(freeBegin->next->next, 0);
    freeBegin->next->next->size = 0;
    freeBegin->next->next->flag = MemInfo::None;
}

void* MemoryManager::Alloc(uint32_t size)
{
    MemInfo* mi;
    mi = freeBegin->next;
    uint32_t size_ = (size + 3) & ~3;
    while (mi != 0) {
        if (mi->size >= size_) {
            // 十分な空領域があった
            MemInfo* alloc = mi;
            if (mi->size <= size_ + sizeof(MemInfo)) {
                // 空領域に新たな区画(MemInfo)を作るほどの余裕はない
                Unlink(mi);
            } else {
                // 新たな区画を1つ作る

                // 新しいMemInfoは後ろを16バイト境界に合わせる
                uintptr_t miFreeAddr = (uintptr_t)mi + sizeof(MemInfo);
                mi = (MemInfo*)(
                        miFreeAddr + Ceil(size_ + sizeof(MemInfo)) - sizeof(MemInfo)
                        );
                uint32_t diff = (uintptr_t)mi - (uintptr_t)alloc;
                mi->size = alloc->size - diff;
                mi->flag = MemInfo::Free;
                alloc->size = diff - sizeof(MemInfo);

                // 線形リストを更新
                Link(alloc->prev, mi);
                Link(mi, alloc->next);
            }

            // 使用領域リストに追加
            Link(alloc, allocBegin->next);
            Link(allocBegin, alloc);
            alloc->flag = MemInfo::Alloc;

            return (void*)((uintptr_t)alloc + sizeof(MemInfo));
        }
        mi = mi->next;
    }

    return 0;
}

void MemoryManager::Free(void* ptr)
{
    if (ptr == 0) {
        return;
    }
    MemInfo* mi = (MemInfo*)((uint32_t)ptr - sizeof(MemInfo));
    if (mi->prev->next != mi || mi->next->prev != mi) {
        /*
         * ptrが間違っている。
         * ptrが正しいアドレスの場合、メモリが破壊されている可能性が高い。
         */
        return;
    }
    if (mi->flag == MemInfo::Alloc) {
        // リストから削除
        Unlink(mi);

        // フラグを空に設定
        mi->flag = MemInfo::Free;

        // 空領域リストの検索
        MemInfo* prev = freeBegin;
        uintptr_t paddr, miaddr, naddr;
        while (prev != 0) {
            paddr = (uintptr_t)prev;
            miaddr = (uintptr_t)mi;
            naddr = (uintptr_t)prev->next;
            if (paddr < miaddr && miaddr < naddr) {
                // 挿入位置が見つかった

                // エラーチェック
                if (paddr + sizeof(MemInfo) + prev->size <= miaddr &&
                        miaddr + sizeof(MemInfo) + mi->size <= naddr) {\
                    MemInfo* next = prev->next;

                    // 空領域リストに追加
                    Link(prev, mi);
                    Link(mi, next);

                    if (paddr + sizeof(MemInfo) + prev->size == miaddr) {
                        // 前と結合できる
                        prev->size += mi->size + sizeof(MemInfo);
                        Link(prev, next);
                        mi = prev;
                        miaddr = (uintptr_t)mi;
                    }
                    if (miaddr + sizeof(MemInfo) + mi->size == naddr) {
                        // 後ろと結合できる
                        mi->size += next->size + sizeof(MemInfo);
                        Link(mi, next->next);
                    }
                }
                return;
            }
            prev = prev->next;
        }
    }
}

uint32_t MemoryManager::GetFreeSize()
{
    MemInfo* mi = freeBegin;
    uint32_t sum = 0;
    while (mi != 0) {
        sum += mi->size;
        mi = mi->next;
    }
    return sum;
}

uint32_t MemoryManager::Floor(uint32_t value)
{
    // 切捨て
    return value & ~Alignment;
}

uint32_t MemoryManager::Ceil(uint32_t value)
{
    // 切り上げ
    return (value + Alignment) & ~Alignment;
}

void MemoryManager::Link(MemInfo* prev, MemInfo* next)
{
    // 2つのMemInfoを繋げる
    if (prev != 0) {
        prev->next = next;
    }
    if (next != 0) {
        next->prev = prev;
    }
}

void MemoryManager::Unlink(MemInfo* mi)
{
    /*
     * リストからmiをはずす
     * リストの両端の要素は指定されないことが前提
     */
    if (mi != 0) {
        Link(mi->prev, mi->next);
    }
}


#elif MEMMAN == 3

const size_t SizeList[5] = { 32, 128, 512, 2048, 0 };
const int CountList[5] = { 32 * 1024, 8 * 1024, 2 * 1024, 512, 0 };

void StaticMemoryManager::List::Init()
{
    this->begin.SetPrev(0);
    this->begin.LinkTo(&this->end);
    this->end.SetNext(0);
    this->count = 0;
}

void StaticMemoryManager::List::Add(MemInfo *item)
{
    if (item != 0) {
        item->LinkTo(this->begin.GetNext());
        this->begin.LinkTo(item);
        this->count++;
    }
}

StaticMemoryManager::MemInfo *StaticMemoryManager::List::Remove(MemInfo *item)
{
    if (item != 0 && item->GetPrev()->GetNext() == item && item->GetNext()->GetPrev() == item) {
        item->GetPrev()->LinkTo(item->GetNext());
        item->Init(0, 0);
        this->count--;
        return item;
    }
    return 0;
}

StaticMemoryManager::MemInfo *StaticMemoryManager::List::RemoveFirst()
{
    return Remove(this->begin.GetNext());
}

void StaticMemoryManager::List::Merge(MemInfo *item)
{
    if (item != 0) {
        MemInfo *prev = &this->begin;
        while (prev->GetNext() != &this->end && prev->GetNext() < item) {
            prev = prev->GetNext();
        }
        item->LinkTo(prev->GetNext());
        prev->LinkTo(item);
        this->count++;

        item = Merge(prev, item);
        item = Merge(item, item->GetNext());
    }
}

void StaticMemoryManager::List::AddRange(MemInfo *begin, MemInfo *end, int count)
{
    if (begin != 0 && end != 0) {
        end->LinkTo(this->begin.GetNext());
        this->begin.LinkTo(begin);
        this->count += count;
    }
}

StaticMemoryManager::MemInfo *StaticMemoryManager::List::Merge(MemInfo *prev, MemInfo *next)
{
    // prevとnextが結合可能？
    if ((uintptr_t)prev + sizeof(MemInfo) + sizeof(size_t) + prev->GetSize() == (uintptr_t)next) {
        prev->SetSize(prev->GetSize() + sizeof(MemInfo) + sizeof(size_t) + next->GetSize());
        prev->LinkTo(next->GetNext());
        this->count--;
        return prev;
    }
    return next;
}


void StaticMemoryManager::Init(uintptr_t begin, size_t size)
{
    uintptr_t end = begin + size;

    begin = (begin + 3) & ~3;
    end = end & ~3;
    this->begin = begin;
    this->end = end;

    MemInfo *mi = (MemInfo*)begin, *next;

    MemInfo *miBegin, *miEnd;

    int sizeKind;

    /*
     * リストの初期化
     */
    for (sizeKind = 0; sizeKind < SizeKind; sizeKind++) {
        freeLists[sizeKind].Init();
        allocLists[sizeKind].Init();
    }

    /*
     * 固定メモリ領域
     */
    for (sizeKind = 0; sizeKind < SizeKind - 1; sizeKind++) {
        miBegin = mi;
        for (int i = 0; i < CountList[sizeKind] - 1; i++) {
            next = (MemInfo*)((uintptr_t)mi + SizeList[sizeKind]);
            mi->LinkTo(next);
            mi = next;
        }
        miEnd = mi;
        freeLists[sizeKind].AddRange(miBegin, miEnd, CountList[sizeKind]);

        mi = (MemInfo*)((uintptr_t)mi + SizeList[sizeKind]);
    }

    /*
     * 可変メモリ領域
     */
    mi->SetSize(end - (uintptr_t)mi - sizeof(MemInfo) - sizeof(size_t));
    freeLists[sizeKind].Add(mi);
}

void* StaticMemoryManager::Alloc(size_t size)
{
    const size_t MemInfoSize = sizeof(MemInfo) + sizeof(size_t);

    uintptr_t result;
    MemInfo *mi = 0;
    List *l;
    int sizeKind, i;

    size = (size + 3) & ~0x00000003;

    for (i = 0; i < SizeKind - 1; i++) {
        if (size <= SizeList[i] - sizeof(MemInfo)) {
            break;
        }
    }
    sizeKind = i;

    result = 0;
    for (; sizeKind < SizeKind - 1; sizeKind++) {
        l = freeLists + sizeKind;
        if (l->GetCount() > 0) {
            // 固定長領域に空きがある
            mi = l->RemoveFirst();
            allocLists[sizeKind].Add(mi);
            return (void*)((uintptr_t)mi + sizeof(MemInfo));
        }
    }
    l = freeLists + SizeKind - 1;
    if (l->GetCount() > 0) {
        // 固定長領域に空きがないか、固定長領域には入りきらないsizeである
        mi = l->GetBegin()->GetNext();
        for (; mi != l->GetEnd(); mi = mi->GetNext()) {
            if (mi->GetSize() >= size) {
                // 十分な大きさの空き領域が見つかった（first fit）

                // 新しくMemInfoを作るかどうか
                if (mi->GetSize() > size + MemInfoSize) {
                    // 新しい領域を作る
                    // newInfoが確保する領域
                    MemInfo *newInfo = (MemInfo*)((uintptr_t)mi + MemInfoSize + mi->GetSize() - size - MemInfoSize);
                    newInfo->SetSize(size);
                    mi->SetSize(mi->GetSize() - size - MemInfoSize);
                    mi = newInfo;
                } else {
                    // １つの空き領域すべてを転用
                    freeLists[SizeKind - 1].Remove(mi);
                }
                allocLists[SizeKind - 1].Add(mi);
                return (void*)((uintptr_t)mi + sizeof(MemInfo));
            }
        }
    }

    return (void*)0;
}

void StaticMemoryManager::Free(void *ptr)
{
    if (this->begin <= (uintptr_t)ptr && (uintptr_t)ptr < this->end) {
        uintptr_t begin = this->begin;
        uintptr_t end;
        int sizeKind;
        MemInfo *mi;

        for (sizeKind = 0; sizeKind < SizeKind - 1; sizeKind++) {
            end = begin + SizeList[sizeKind] * CountList[sizeKind];
            if ((uintptr_t)ptr < end) {
                // ptrが固定領域のポインタだった
                mi = (MemInfo*)((uintptr_t)ptr - sizeof(MemInfo));

                allocLists[sizeKind].Remove(mi);
                freeLists[sizeKind].Add(mi);
                goto free_end;
            }
            begin = end;
        }
        // ptrは可変長領域のポインタだった
        mi = (MemInfo*)((uintptr_t)ptr - sizeof(MemInfo) - sizeof(size_t));
        mi = allocLists[sizeKind].Remove(mi);
        freeLists[sizeKind].Merge(mi);
        goto free_end;

free_end:
        return;
    }

}

size_t StaticMemoryManager::GetFreeSize()
{
    size_t sum = 0;
    int sizeKind;
    for (sizeKind = 0; sizeKind < SizeKind - 1; sizeKind++) {
        sum += (SizeList[sizeKind] - sizeof(MemInfo)) * freeLists[sizeKind].GetCount();
    }
    MemInfo *mi = freeLists[sizeKind].GetBegin()->GetNext();
    while (mi != freeLists[sizeKind].GetEnd()) {
        sum += mi->GetSize();
        mi = mi->GetNext();
    }
    return sum;
}


#endif

MemoryManager memman;

void* operator new(size_t size)
{
    return memman.Alloc(size);
}

void* operator new[](size_t size)
{
    return memman.Alloc(size);
}

void operator delete(void* ptr)
{
    memman.Free(ptr);
}

void operator delete[](void* ptr)
{
    memman.Free(ptr);
}


/*
 * memcpy is imported from BayOS
 *
 * this source's license is MIT License
 */
/*
   Copyright (c) 2004-2007 bayside

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation files
   (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   */

void* memcpy(void* s1, const void* s2, size_t size) {
    int count4 = size / 4;
    int count1 = size - count4 * 4;

    /* 4バイト転送部分 */
    asm volatile("movl %0, %%edi \n"
            "movl %1, %%esi \n"
            "movl %2, %%ecx \n"
            "cld            \n"
            "rep movsd      \n"
            :
            : "m"(s1), "m"(s2), "m"(count4)
            : "edi", "esi", "ecx");

    /* 1バイト転送部分 */
    asm volatile("movl %0, %%ecx \n"
            "rep movsb      \n"
            :
            : "m"(count1)
            : "ecx");

    return s1;
}

uint32_t physicalMemorySize;

