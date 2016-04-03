/*
 * memory.h
 *
 *  Created on: 2009/07/08
 *      Author: uchan
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>
#include <stddef.h>

/*
 * 物理メモリの総容量を計算する
 */
uintptr_t GetPhysicalMemoryCapacity(uintptr_t begin, uintptr_t end);

#define MEMMAN 3

#if MEMMAN == 1
/*
 * メモリ使用情報を入れておく
 */
struct MemInfo
{
    uintptr_t addr;
        uint32_t size;
        MemInfo* next;
};

/*
 * メモリ管理クラス
 */
class MemoryManager : private Uncopyable
{
    private:
            // メモリ管理情報の個数
            static const int MemInfoSize = 65536;

                // allocするときのアライメント - 1
                static const int Alignment = 16 - 1;

    private:
            // メモリ管理情報の実態
            //MemInfo mi0[MemInfoSize];
            MemInfo* mi0;

                // 未使用メモリ管理情報リストの先頭
                MemInfo* miInvalidBegin;

                // 空き領域リストの先頭 (アドレスの昇順にソートされている)
                MemInfo* miFreeBegin;

                // 使用中領域リストの先頭 (ソートされていない)
                MemInfo* miAllocBegin;

    public:
           // メモリマネージャの初期化
           void Init(uintptr_t start, uintptr_t end, MemInfo* memInfoBuf);

               // 割り当て
               void* Alloc(size_t size);

               // 開放
               void Free(void* ptr);

    private:
            // 未使用メモリ管理情報構造体を1つ割り当てる
            MemInfo* MemInfoAlloc();

                // 指定したメモリ管理情報を未使用にする
                void MemInfoFree(MemInfo* mi);

                // 空き領域リストに追加する
                void FreeListAdd(MemInfo* mi);

                // 指定したサイズのメモリ管理情報を生成して返す
                MemInfo* FreeListSearch(size_t size);

                // 使用中領域リストに追加する
                void AllocListAdd(MemInfo* mi);

                // 指定した領域を空き領域にする
                MemInfo* AllocListRemove(uintptr_t addr);
};

#elif MEMMAN == 2


class MemInfo
{
    friend class MemoryManager;

        enum Flags {
            Free, Alloc, None
        };

        MemInfo* prev;
        uint32_t size;
        Flags flag;
        MemInfo* next;
};

class MemoryManager
{
    // メモリ割り当てのアライメント
    static const int Alignment = 16 - 1;

        MemInfo* freeBegin; // 空領域リストの先頭
    MemInfo* allocBegin; // 使用領域リストの先頭

        uintptr_t memBegin; // 管理領域の先頭アドレス
    uint32_t memSize; // 管理領域のサイズ
    public:
           // メモリマネージャの初期化
           void Init(uintptr_t begin, uint32_t size);

               // メモリ割り当て
               void* Alloc(uint32_t size);

               // メモリ開放
               void Free(void* ptr);

               // 空き領域取得
               uint32_t GetFreeSize();

    private:
            // Alignmentの切り上げ
            uint32_t Floor(uint32_t value);

                // Alignmentの切り捨て
                uint32_t Ceil(uint32_t value);

                // 線形リストに繋ぐ
                void Link(MemInfo* prev, MemInfo* next);

                // 線形リストからはずす
                void Unlink(MemInfo* mi);
};

#elif MEMMAN == 3

extern const size_t SizeList[5];
extern const int CountList[5];

class StaticMemoryManager
{
    uintptr_t begin, end;

        class MemInfo
        {
            MemInfo* prev;
                MemInfo* next;
            public:
                   MemInfo* GetPrev()
                   {
                       return this->prev;
                   }
                   void SetPrev(MemInfo* prev)
                   {
                       this->prev = prev;
                   }
                   MemInfo* GetNext()
                   {
                       return this->next;
                   }
                   void SetNext(MemInfo* next)
                   {
                       this->next = next;
                   }
                   void LinkTo(MemInfo* mi)
                   {
                       this->next = mi;
                           if (mi != 0) {
                               mi->prev = this;
                           }
                   }
                   void Init(MemInfo* prev, MemInfo* next)
                   {
                       this->prev = prev;
                           this->next = next;
                   }

                       uintptr_t GetEndAddress()
                       {
                           return (uintptr_t)this + sizeof(MemInfo);
                       }
                   size_t GetSize()
                   {
                       return ((size_t*)GetEndAddress())[0];
                   }
                   void SetSize(size_t size)
                   {
                       ((size_t*)GetEndAddress())[0] = size;
                   }
        };

        class List
        {
            MemInfo begin;
                MemInfo end;
                int count;
            public:

                       void Init();

                       /*
                        * itemをリストの先頭に加える。
                        */
                       void Add(MemInfo *item);

                       /*
                        * itemをリストから削除する。
                        */
                       MemInfo *Remove(MemInfo *item);

                       /*
                        * 先頭の要素をリストから削除する。
                        */
                       MemInfo *RemoveFirst();

                       /*
                        * itemをアドレス順になるようにリストに加え、
                        * 前後と結合できるならする。
                        */
                       void Merge(MemInfo *item);

                       /*
                        * リストをリストの先頭に加える。
                        */
                       void AddRange(MemInfo *begin, MemInfo *end, int count);

                       /*
                        * リストの要素数を返す。
                        */
                       int GetCount()
                       {
                           return this->count;
                       }

                       MemInfo *GetBegin()
                       {
                           return &this->begin;
                       }

                       MemInfo *GetEnd()
                       {
                           return &this->end;
                       }

            private:
                    /*
                     * prevとnextが結合できるならする。
                     * 結合したならprevを、結合していないならnextを返す。
                     */
                    MemInfo* Merge(MemInfo *prev, MemInfo *next);
        };

        static const int SizeKind = sizeof(SizeList) / sizeof(SizeList[0]);

        List allocLists[SizeKind];
        List freeLists[SizeKind];

    public:
           /*
            * 管理する領域の情報を与える。
            */
           void Init(uintptr_t begin, size_t size);

               /*
                * 指定されたサイズ以上の領域を確保して返す。
                */
               void* Alloc(size_t size);

               /*
                * 指定された領域を解放する。
                */
               void Free(void *ptr);

               /*
                * 空き領域を計算する
                */
               size_t GetFreeSize();

};

typedef StaticMemoryManager MemoryManager;

#endif

extern MemoryManager memman;

extern uint32_t physicalMemorySize;

#endif /* MEMORY_H_ */
