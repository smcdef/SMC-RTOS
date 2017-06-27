/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-04
 * Describe: SMC-RTOS core code
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_CORE_H
#define SMC_CORE_H

#include "smc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern smc_uint32_t smc_bitmap_group;                     /* thread priority bit map */

/**
 * The function will set a bit according to thread priority
 *
 * @param prio [the thread priority]
 */
#define smc_bitmap_set(prio)      (smc_bitmap_group |= (1 << prio))

/**
 * The function will clear a bit according to thread priority
 *
 * @param prio [the thread priority]
 */
#define smc_bitmap_clear(prio)    (smc_bitmap_group &= ~(1 << prio))

/**
 * The function will get the highest priority
 */
smc_uint8_t smc_get_highest_prio(void);

/**
 * The function will make a timer counter decrease and thread slice tick decrease,
 * it should be invoked in interrupt handle.
 */
void smc_time_tick(void);

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it.
 */
void smc_scheduler(void);

/**
 * This function will init SMC-RTOS
 */
void smc_rtos_init(void);

/**
 * This function will startup SMC-RTOS
 */
void smc_rtos_scheduler(void);

/**
 * This function sets a hook function to thread scheduler. When the system performs 
 * thread scheduler, this hook function should be invoked.
 *
 * @param hook [the specified hook function]
 *
 * @note       [the hook function must be simple and never be blocked or suspend.]
 */
void smc_scheduler_sethook(void (*hook)(void));

/**
 * This function will be invoked by BSP, when enter interrupt service routine
 *
 * @note [please don't invoke this routine in application]
 *
 */
void smc_enter_interrupt(void);

/**
 * This function will be invoked by BSP, when leave interrupt service routine
 *
 * @note [please don't invoke this routine in application]
 *
 */
void smc_exit_interrupt(void);

/**
 * Using cpu usage for SMC-RTOS
 */
#ifdef SMC_USING_CPU_USAGE

/**
 * This function will return cpu usage
 *
 * @return  [cpu usage]
 */
smc_uint8_t smc_get_cpu_usage(void);

#endif

#ifdef __cplusplus
}
#endif

#endif // SMC_CORE_H
