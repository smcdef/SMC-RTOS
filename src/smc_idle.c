/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-12
 * Describe: This is part of SMC-RTOS for idle thread
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_idle.h"
#include "smc_thread.h"

/**
 * The idle thread stack definition
 */
static struct smc_thread smc_thread_idle;
static smc_uint8_t smc_idle_thread_stack[SMC_IDLE_STACK_SIZE];
static void (*smc_thread_idle_hook)(void) = NULL;

/**
 * This function sets a hook function to idle thread loop. When the system performs 
 * idle loop, this hook function should be invoked.
 *
 * @param hook [the specified hook function]
 *
 * @note       [the hook function must be simple and never be blocked or suspend.]
 */
void smc_thread_idle_sethook(void (*hook)(void))
{
    smc_thread_idle_hook = hook;
}

/**
 * Idle thread init
 */
void smc_idle_thread_init(void)
{
	smc_thread_init(&smc_thread_idle,
	                idle_thread_entry,
	                NULL,
	                IDLE_THREAD_PRIORITY,
	                smc_idle_thread_stack,
	                SMC_IDLE_STACK_SIZE,
	                40);
}

/**
 * Idle thread entry with the parameter
 *
 * @param parameter [the parameter of thread enter function]
 */
void idle_thread_entry(void *parameter)
{
	while (1) {
		if (smc_thread_idle_hook)
			smc_thread_idle_hook();
	}
}
