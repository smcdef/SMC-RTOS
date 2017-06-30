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
#include "smc_timer.h"

/**
 * The idle thread stack definition
 */
static struct smc_thread smc_thread_idle;
static smc_uint8_t smc_idle_thread_stack[SMC_IDLE_STACK_SIZE];
static void (*smc_thread_idle_hook)(void) = NULL;

/**
 * Using cpu usage for SMC-RTOS
 */
#ifdef SMC_USING_CPU_USAGE

static smc_uint8_t smc_cpu_usage     = 0;      /* Percentage of cpu used                       */
static smc_uint32_t smc_idle_cnt_max = 0;      /* Max value that idle cnt can take in 1 sec    */
static smc_uint32_t smc_idle_cnt_run = 0;      /* Val reached by idle cnt at run time in 1 sec */

static smc_timer_t smc_idle_timer;             /* smc_idle_timer computes the cpu usage        */

/**
 * This function will computes the percentage of cpu usage
 *
 * @param parameter [NULL]
 */
static void smc_idle_timeout(void *parameter)
{
	/**
	 * For the first time, get the the maximum value(smc_idle_cnt_max) for the idle counter.
	 */
	if (smc_idle_cnt_max == 0U) {
		smc_uint32_t tick = SMC_TICKS_PER_SECOND;

		smc_idle_cnt_max = smc_idle_cnt_run / 10U;

		/* Set timer timeout tick for 1 second */
		smc_timer_command(&smc_idle_timer,
		                  SMC_TIMER_SET_TIMEOUT_TICK_IMMEDIATELY,
		                  &tick);

		/* do scheduler */
		smc_scheduler_unlock();
	} else {
		smc_int8_t usage;

		/* Computes the cpu usage */
		usage         = (smc_int8_t)(100U - smc_idle_cnt_run / smc_idle_cnt_max);
		smc_cpu_usage = usage > 0 ? usage : 0;
	}
	/* Reset the idle counter for the next second */
	smc_idle_cnt_run = 0;
}

/**
 * This function will lock scheduler, and create a timer for 100ms to
 * establish the maximum value for the idle counter.
 */
static void smc_cpu_usage_init(void)
{
	/* Scheduler lock to establish the maximum value for the idle counter */
	smc_scheduler_lock();

	/* Create a timer for 100ms to establish the maximum value for the idle counter in 100ms */
	smc_timer_init(&smc_idle_timer,
	               SMC_TICKS_PER_SECOND / 10,
	               smc_idle_timeout,
	               NULL,
	               SMC_TIMER_PERIODIC);

	/* start timer */
	smc_timer_enable(&smc_idle_timer);
}

/**
 * This function will return percentage of cpu usage
 *
 * @return  [Percentage of cpu usage]
 */
smc_uint8_t smc_get_cpu_usage(void)
{
	return smc_cpu_usage;
}

#endif
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
/**
 * Using cpu usage for SMC-RTOS
 */
#ifdef SMC_USING_CPU_USAGE
	smc_cpu_usage_init();
#endif

	while (1) {
/**
 * Using cpu usage for SMC-RTOS
 */
#ifdef SMC_USING_CPU_USAGE
		smc_int32_t status;

		/* disable interrupt */
		status = smc_cpu_disable_interrupt();
		smc_idle_cnt_run++;
		/* enable interrupt */
		smc_cpu_enable_interrupt(status);
#endif
		if (smc_thread_idle_hook)
			smc_thread_idle_hook();
	}
}
