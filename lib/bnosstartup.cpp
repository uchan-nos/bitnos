/*
 * bnosstartup.cpp
 *
 *  Created on: 2009/07/05
 *      Author: uchan
 */

extern "C"
{
  void BNosMain();
  void BNosStartup();
}

void BNosStartup()
{
  BNosMain();
}
