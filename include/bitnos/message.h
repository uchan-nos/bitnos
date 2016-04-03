/*
 * message.h
 *
 *  Created on: 2009/07/13
 *      Author: uchan
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>
#include <bitnos/uncopyable.h>

struct Message
{
    struct From : private Uncopyable
    {
        static const uint32_t Unknown  = 0x00000000;
        static const uint32_t System   = 0x00000001;
        static const uint32_t Keyboard = 0x00000002;
        static const uint32_t Mouse    = 0x00000003;
        static const uint32_t Timer    = 0x00000004;
        static const uint32_t Int0d    = 0x00000005;
    };

    struct System : private Uncopyable
    {
        static const uint32_t WindowActivated   = 0x00000001;
        static const uint32_t WindowInactivated = 0x00000002;
    };

    // メッセージの種類
    uint32_t from;

    // メッセージの内容
    uint32_t arg;
    void *ptr;

    Message(
            uint32_t from = 0,
            uint32_t arg = 0, void *ptr = 0);
};

#endif /* MESSAGE_H_ */
