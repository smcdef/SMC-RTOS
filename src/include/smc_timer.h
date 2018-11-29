/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-13
 * Describe: This is a part of SMC-RTOS for timer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_TIMER_H
#define SMC_TIMER_H

#include "smc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function will init a timer
 *
 * @param timer     [the timer to be initialized]
 * @param tick      [the tick to be delay]
 * @param timerout  [the function will be invoked when its delay time comes]
 * @param parameter [the function parameter]
 * @param flag      [the timer operater mode flag]
 */
void smc_timer_init(smc_timer_t *timer,
                    smc_uint32_t tick,
                    void (*timerout)(void *parameter),
                    void *parameter,
                    smc_uint8_t flag);

/**
 * This function will enable the special timer
 *
 * @param timer [the timer will be enable]
 */
void smc_timer_enable(smc_timer_t *timer);

/**
 * This function will disable the special timer
 *
 * @param timer [the timer will be disable]
 */
void smc_timer_disable(smc_timer_t *timer_del);

/**
 * The function will set different parameters according to different commands
 *
 * @param timer [the timer to be set]
 * @param cmd   [the command]
 * @param arg   [the argument]
 */
void smc_timer_command(smc_timer_t *timer, smc_uint8_t cmd, void *arg);

/**
 * The function will make a timer counter decrease, and should be invoked
 * in interrupt handle.
 */
void smc_timer_decrease(void);

#ifdef __cplusplus
}
#endif

#endif // SMC_TIMER_H
