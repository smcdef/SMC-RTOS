/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-03
 * Describe: The following functions need to be implemented by the user
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_CPU_H
#define SMC_CPU_H

#include "smc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function will return current system interrupt status and disable system
 * interrupt.
 *
 * @return [the current system interrupt status]
 */
smc_uint32_t smc_cpu_disable_interrupt(void);

/**
 * This function will set the specified interrupt status, which shall saved by
 * smc_cpu_disable_interrupt function. If the saved interrupt status is interrupt
 * opened, this function will open system interrupt status.
 */
void smc_cpu_enable_interrupt(smc_uint32_t status);

/**
 * This function will initialize thread stack
 *
 * @param tentry     [the entry of thread]
 * @param parameter  [the parameter of entry]
 * @param stack_addr [the beginning stack address]
 *
 * @return stack address
 */
smc_stack_t *smc_thread_stack_init(void (*entry)(void *parameter),
                                   void *parameter,
                                   smc_stack_t *stack_addr);
/**
 * This function will make context switch.
 *
 * @note [switch not in interrupt]
 *
 */
void smc_thread_switch(void);
								   
/**
 * This function will make context switch.
 *
 * @note [switch in interrupt]
 *
 */
void smc_thread_intrrupt_switch(void);
								   
/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it. It only for the
 * first context switch
 */
void smc_thread_switch_to(void);
								   
								   
#ifdef __cplusplus
}
#endif

#endif // SMC_CPU_H
