/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-12
 * Describe: This is part of SMC-RTOS for idle thread
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_IDLE_H
#define SMC_IDLE_H

#include "smc_def.h"

#define IDLE_THREAD_PRIORITY    (SMC_PRIORITY_MAX - 1)       /* idle thread prority */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Idle thread entry with the parameter
 *
 * @param parameter [the parameter of thread enter function]
 */
void idle_thread_entry(void *parameter);

#ifdef __cplusplus
}
#endif


#endif // SMC_IDLE_H
