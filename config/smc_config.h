/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-06
 * Describe: user configuration for SMC-RTOS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_CONFIG_H
#define SMC_CONFIG_H

#define SMC_TICKS_PER_SECOND             200  /* How many ticks are there in a second */
#define SMC_PRIORITY_MAX                 32   /* SMC-RTOS support 32 priority for max */
#define SMC_IDLE_STACK_SIZE              512  /* how many bytes for idle thread stack size */

/**
 * user module configration
 */
#define SMC_USING_SEMAPHORE                   /* using semaphore for SMC-RTOS */

#endif // SMC_CONFIG_H
