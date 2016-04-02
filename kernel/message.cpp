/*
 * message.cpp
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#include <bitnos/message.h>

Message::Message(
        uint32_t from,
        uint32_t arg, void *ptr) :
    from(from), arg(arg), ptr(ptr)
{
}

