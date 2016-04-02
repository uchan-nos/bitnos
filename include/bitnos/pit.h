/*
 * pit.h
 *
 *  Created on: 2009/08/11
 *      Author: uchan
 */

#ifndef PIT_H_
#define PIT_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>
#include <bitnos/timer.h>

extern "C"
{
  void inthandler20(int* esp);
}

class PIT : private Uncopyable
{
private:
  struct Port : private Uncopyable
  {
    static const uint16_t Cnt0   = 0x0040;
    static const uint16_t Cnt1   = 0x0041;
    static const uint16_t Cnt2   = 0x0042;
    static const uint16_t CR     = 0x0043;
    static const uint16_t System = 0x0061;
  };

  struct CountMode : private Uncopyable
  {
    static const uint16_t M0Bin = 0x0;
    static const uint16_t M1Bin = 0x2;
    static const uint16_t M2Bin = 0x4;
    static const uint16_t M3Bin = 0x6;
    static const uint16_t M4Bin = 0x8;
    static const uint16_t M5Bin = 0xa;
  };

  struct CountAccessMode : private Uncopyable
  {
    static const uint16_t Latch = 0x0 << 4;
    static const uint16_t L     = 0x1 << 4;
    static const uint16_t H     = 0x2 << 4;
    static const uint16_t LH    = 0x3 << 4;
  };

  struct CH : private Uncopyable
  {
    static const uint16_t CH0      = 0x0 << 6;
    static const uint16_t CH1      = 0x1 << 6;
    static const uint16_t CH2      = 0x2 << 6;
    static const uint16_t ReadBack = 0x3 << 6;
  };
public:

  static void Init();

  static void IntHandler();

  static void BeetSetCounter(int cnt);

  static void BeepOn();

  static void BeepOff();
};

#endif /* PIT_H_ */
