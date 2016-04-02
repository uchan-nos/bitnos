/*
 * queue.h
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <bitnos/debug.h>

template <typename T>
class Queue
{
private:
  T* buf; // データを格納するバッファ
  int bufsize; // バッファのサイズ
  int rp; // 読み込み位置
  int wp; // 書き込み位置
  int count; // 要素数
  int lost; // あふれた数
  bool allocated; // newしたかどうか

  void Init(T* buf, int bufsize, bool allocated);

public:
  Queue(int bufsize);

  Queue(T* buf, int bufsize);

  ~Queue();

  void PushBack(const T& data);

  T& Front();

  void PopFront();

  T& Back();

  int GetCount() const
  {
    return count;
  }

  int GetLost() const
  {
    return lost;
  }

  int GetFree() const
  {
    return bufsize - count;
  }

  bool Remove(const T& data);

  bool RemoveAt(int index);

private:
  void IncWP()
  {
    wp++;
    if (wp >= bufsize) {
      wp = 0;
    }
    count++;
  }

  void IncRP()
  {
    rp++;
    if (rp >= bufsize) {
      rp = 0;
    }
    count--;
  }

  void DecWP()
  {
    wp--;
    if (wp < 0) {
      wp = bufsize - 1;
    }
    count--;
  }
};

template <typename T>
void Queue<T>::Init(T* buf, int bufsize, bool allocated)
{
  this->rp = 0;
  this->wp = 0;
  this->count = 0;
  this->lost = 0;
  this->buf = buf;
  this->bufsize = bufsize;
  this->allocated = allocated;
}

template <typename T>
Queue<T>::Queue(int bufsize)
{
  T* buf = new T[bufsize];
  if (buf != 0) {
    this->Init(buf, bufsize, true);
  } else {
    this->Init(0, 0, false);
  }
}

template <typename T>
Queue<T>::Queue(T* buf, int bufsize)
{
  this->Init(buf, bufsize, false);
}

template <typename T>
Queue<T>::~Queue()
{
  if (this->allocated) {
    delete[] this->buf;
  }
}

template <typename T>
void Queue<T>::PushBack(const T& data)
{
  if (count < bufsize) {
    buf[wp] = data;
    IncWP();
  } else {
    lost++;
  }
}

template <typename T>
T& Queue<T>::Front()
{
  return buf[rp];
}

template <typename T>
void Queue<T>::PopFront()
{
  if (count > 0) {
    IncRP();
  }
}

template <typename T>
T& Queue<T>::Back()
{
  int i = wp - 1;
  if (i < 0) {
    i += bufsize;
  }
  return buf[i];
}

template <typename T>
bool Queue<T>::Remove(const T& data)
{
  int i, j;
  for (i = 0, j = rp; i < count; i++, j++) {
    if (j >= bufsize) {
      j -= bufsize;
    }
    if (buf[j] == data) {
      return RemoveAt(j);
    }
  }
  return false;
}

template <typename T>
bool Queue<T>::RemoveAt(int index)
{
  if (0 <= index && index < bufsize) {
    DecWP();

    int i, j, k, end = wp - index;
    if (end < 0) {
      end += bufsize;
    }
    for (i = 0, j = index, k = index + 1; i < end; i++, j++, k++) {
      if (j >= bufsize) {
        j -= bufsize;
      } else if (k >= bufsize) {
        k -= bufsize;
      }
      buf[j] = buf[k];
    }
    return true;
  }
  return false;
}



#endif /* QUEUE_H_ */
