/*
 * cpuexceptions.h
 *
 *  Created on: 2009/08/17
 *      Author: uchan
 */

#ifndef CPUEXCEPTIONS_H_
#define CPUEXCEPTIONS_H_

#include <stdint.h>

extern "C" {
    uint32_t* inthandler01(uint32_t* esp);
    uint32_t* inthandler0d(uint32_t* esp);
}

#endif /* CPUEXCEPTIONS_H_ */
