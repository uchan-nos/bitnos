/*
 * bitnos.h
 *
 *  Created on: 2009/07/05
 *      Author: uchan
 */

#ifndef BITNOS_H_
#define BITNOS_H_

#include <stdint.h>
#include <stddef.h>
#include <bitnos/uncopyable.h>
#include <bitnos/point.h>
#include <bitnos/queue.h>
#include <bitnos/funcs.h>
#include <bitnos/bootinfo.h>
#include <bitnos/message.h>
#include <bitnos/font.h>
#include <bitnos/IDraw.h>
#include <bitnos/image.h>
#include <bitnos/memory.h>
#include <bitnos/debug.h>
#include <bitnos/desctable.h>
#include <bitnos/int.h>
#include <bitnos/kbc.h>
#include <bitnos/keyboard.h>
#include <bitnos/sheet.h>
#include <bitnos/mouse.h>
#include <bitnos/timer.h>
#include <bitnos/pit.h>
#include <bitnos/mtask.h>
#include <bitnos/specialtask.h>
#include <bitnos/window.h>
#include <bitnos/keycode.h>
#include <bitnos/console.h>

extern "C"
{
    void BNosMain();
    void BNosStartup();
}

#endif /* BITNOS_H_ */
