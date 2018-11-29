/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-04
 * Describe: SMC-RTOS core code
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_core.h"
#include "smc_thread.h"
#include "smc_list.h"
#include "smc_timer.h"
#include "smc_cpu.h"

static void (*smc_scheduler_hook)(void);
smc_uint32_t smc_bitmap_group;                     /* thread priority bit map */
static smc_uint8_t smc_scheduler_lock_count;       /* the scheduler lock nest */
static volatile smc_uint8_t smc_interrupt_nest;

/**
 * This function finds the first bit set (beginning with the least significant bit)
 * in value and return the index of that bit.
 *
 * Bits are numbered starting at 1 (the least significant bit).  A return value of
 * zero from any of these functions means that the argument was zero.
 *
 * @return return the index of the first bit set. If value is 0, then this function
 * shall return 0.
 */
__asm static smc_uint32_t __bit_search(smc_uint32_t value)
{
	RBIT    R0, R0                            /* reversal RO for bit */
	CLZ     R0, R0                            /* Count Leading Zeros */
	BX      LR
}

/**
 * The function will get the highest priority
 */
smc_uint8_t smc_get_highest_prio(void)
{
	return __bit_search(smc_bitmap_group);
}

/**
 * The function will make a timer counter decrease and thread slice tick decrease,
 * it should be invoked in interrupt handle.
 */
void smc_time_tick(void)
{
	smc_timer_decrease();
	smc_thread_current->remaining_slice_tick--;
	if (smc_thread_current->remaining_slice_tick == 0U) {
		smc_thread_current->remaining_slice_tick = smc_thread_current->init_slice_tick;
		smc_thread_abandon();
	}
}

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it.
 */
void smc_scheduler(void)
{
	smc_uint32_t status;

	if (smc_scheduler_lock_count)
		return;

	status = smc_cpu_disable_interrupt();
	smc_thread_ready = smc_thread_highest_ready();
	smc_cpu_enable_interrupt(status);

	/* if the destination thread is not the same as current thread */
	if (smc_thread_current != smc_thread_ready) {
		if (smc_scheduler_hook)
			smc_scheduler_hook();
		/* switch to new thread */
		if (smc_interrupt_nest > 0U)
			smc_thread_intrrupt_switch();
		else
			smc_thread_switch();
	}
}

/**
 * This function will lock the thread scheduler.
 */
void smc_scheduler_lock(void)
{
	if (smc_scheduler_lock_count < 255U) {
		smc_int32_t status = smc_cpu_disable_interrupt();
		smc_scheduler_lock_count++;
		smc_cpu_enable_interrupt(status);
	}
}

/**
 * This function will unlock the thread scheduler.
 */
void smc_scheduler_unlock(void)
{
	if (smc_scheduler_lock_count > 0U) {
		smc_int32_t status = smc_cpu_disable_interrupt();
		smc_scheduler_lock_count--;
		smc_cpu_enable_interrupt(status);
		if (smc_scheduler_lock_count == 0U)
			smc_scheduler();
	}
}

/**
 * This function will init SMC-RTOS
 */
void smc_rtos_init(void)
{
	smc_uint16_t i;

	for (i = 0; i < SMC_PRIORITY_MAX; i++)
		smc_list_node_init(&smc_list_head_table[i]);
}

/**
 * This function will startup SMC-RTOS
 */
void smc_rtos_scheduler(void)
{
	smc_thread_current = NULL;

	/**
	 * If using cpu usage module, the first thread which will run is idle thread to
	 * establish the maximum value for the idle counter.
	 */
#ifdef SMC_USING_CPU_USAGE
	smc_thread_ready = smc_list_entry(smc_list_head_table[SMC_PRIORITY_MAX - 1].next,
	                                  smc_thread_t,
	                                  rlist);
#else
	smc_thread_ready = smc_thread_highest_ready();
#endif

	/* It only for the first context switch */
	smc_thread_switch_to();
}

/**
 * This function sets a hook function to thread scheduler. When the system performs
 * thread scheduler, this hook function should be invoked.
 *
 * @param hook [the specified hook function]
 *
 * @note       [the hook function must be simple and never be blocked or suspend.]
 */
void smc_scheduler_sethook(void (*hook)(void))
{
	smc_scheduler_hook = hook;
}

/**
 * This function will be invoked by BSP, when enter interrupt service routine
 *
 * @note [please don't invoke this routine in application]
 *
 */
void smc_enter_interrupt(void)
{
	smc_int32_t status;

	/* disable intrrupt */
	status = smc_cpu_disable_interrupt();
	smc_interrupt_nest++;
	/* enable intrrupt */
	smc_cpu_enable_interrupt(status);
}

/**
 * This function will be invoked by BSP, when leave interrupt service routine
 *
 * @note [please don't invoke this routine in application]
 *
 */
void smc_exit_interrupt(void)
{
	smc_int32_t status;

	/* disable intrrupt */
	status = smc_cpu_disable_interrupt();
	smc_interrupt_nest--;
	/* enable intrrupt */
	smc_cpu_enable_interrupt(status);
}

