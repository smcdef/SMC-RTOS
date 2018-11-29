/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-03
 * Describe: thread init for SMC-RTOS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_thread.h"
#include "smc_core.h"
#include "smc_timer.h"

smc_thread_t *smc_thread_current;                        /* point to current thread structure          */
smc_thread_t *smc_thread_ready;                          /* point to highest priority thread structure */
smc_list_head_t smc_list_head_table[SMC_PRIORITY_MAX];   /* ready thread header node for each priority */

/**
 * This function is the timeout function for thread, normally which is invoked
 * when thread is timeout to wait some resource.
 *
 * @param parameter [the parameter of thread timeout function]
 */
void smc_thread_timeout(void *parameter)
{
	smc_thread_t *thread = (smc_thread_t *)parameter;

	/* set thread error number */
	thread->error_num = -SMC_TIMEOUT;

	smc_thread_resume(thread);
}

/**
 * This function will initialize a thread, normally it's used to initialize a
 * static thread object.
 *
 * @param thread      [the static thread object]
 * @param entry       [the entry function of thread]
 * @param parameter   [the parameter of thread enter function]
 * @param priority    [the priority of thread]
 * @param stack_start [the start address of thread stack]
 * @param stack_size  [the size of thread stack]
 * @param slice_tick  [the time slice if there are same priority thread]
 */
void smc_thread_init(smc_thread_t *thread,
                     void (*entry)(void *parameter),
                     void *parameter,
                     smc_uint8_t priority,
                     void *stack_start,
                     smc_uint32_t stack_size,
                     smc_uint32_t slice_tick)
{
	smc_stack_t *stack_end = (smc_stack_t *)((smc_int8_t *)stack_start + stack_size);

	/* Align the stack to 4-bytes */
	thread->sp = smc_thread_stack_init(entry, parameter,
	                                   (smc_stack_t *)SMC_ALIGN_DOWN((smc_stack_t)stack_end, 4));
	thread->priority             = priority;
	thread->init_slice_tick      = slice_tick;
	thread->remaining_slice_tick = slice_tick;
	thread->stat                 = SMC_THREAD_READY;
	thread->error_num            = SMC_OK;

	smc_timer_init(&thread->timer, 0, smc_thread_timeout, thread, SMC_TIMER_DISABLE);
	smc_list_add(&thread->rlist, &smc_list_head_table[priority]);
	smc_bitmap_set(priority);
}

/**
 * This function will let current thread abandon processor, and scheduler will
 * choose a highest thread to run. After abandon processor, the current thread
 * is still in READY queue.
 */
void smc_thread_abandon(void)
{
	smc_uint32_t status = smc_cpu_disable_interrupt();

	if (!smc_list_is_empty(&smc_thread_current->rlist)) {
		/* delete thread from thread list */
		smc_list_del_entry(&smc_thread_current->rlist);

		/* put thread to end of ready queue */
		smc_list_add_tail(&(smc_thread_current->rlist),
		                  &(smc_list_head_table[smc_thread_current->priority]));
		smc_scheduler();
	}

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);
}

/**
 * This function will let current thread delay for some ticks.
 *
 * @param delay_tick [the delay ticks]
 */
void smc_thread_delay(smc_uint32_t delay_tick)
{
	smc_uint8_t flag = SMC_TIMER_ONCE;

	smc_thread_suspend(smc_thread_current);

	/* set timer timeout tick */
	smc_timer_command(&smc_thread_current->timer,
	                  SMC_TIMER_SET_TIMEOUT_TICK_IMMEDIATELY,
	                  &delay_tick);

	/* set timer flag */
	smc_timer_command(&smc_thread_current->timer,
	                  SMC_TIMER_SET_OPERATION_MODE,
	                  &flag);

	/* timer startup */
	smc_timer_enable(&smc_thread_current->timer);

	smc_scheduler();
}

/**
 * This function wiil return the thread with the highest priority level.
 *
 * @return  [thread of the highest priority]
 */
smc_thread_t *smc_thread_highest_ready(void)
{
	smc_uint8_t highest_priority_ready = smc_get_highest_prio();

	return smc_list_entry(smc_list_head_table[highest_priority_ready].next,
	                      smc_thread_t,
	                      rlist);
}

/**
 * This function will suspend the specified thread.
 *
 * @param thread [the thread to be suspended]
 *
 * @return       [the operation status, SMC_OK on OK, -SMC_ERROR on error]
 *
 * @note         [if suspend self thread, after this function call, the]
 * smc_scheduler() must be invoked.
 */
smc_int32_t smc_thread_suspend(smc_thread_t *thread)
{
	smc_uint32_t status;

	status = smc_cpu_disable_interrupt();
	thread->stat = SMC_THREAD_SUSPEND;

	/* delete thread from ready thread queue */
	smc_list_del_entry(&thread->rlist);

	/**
	 * remove current thread timer from timer list
	 */
	smc_timer_disable(&thread->timer);

	/* if current thread ready queue is empty, just clear it's bitmap */
	if (smc_list_is_empty(&smc_list_head_table[thread->priority])) {
		smc_bitmap_clear(thread->priority);
	}
	smc_cpu_enable_interrupt(status);

	return SMC_OK;
}

/**
 * This function will resume a thread and put it to system ready queue.
 *
 * @param thread [the thread to be resumed]
 *
 * @return       [the operation status, SMC_OK on OK, -SMC_ERROR on error]
 */
smc_int32_t smc_thread_resume(smc_thread_t *thread)
{
	smc_uint32_t status;

	if (thread->stat != SMC_THREAD_SUSPEND)
		return -SMC_ERROR;

	status = smc_cpu_disable_interrupt();
	thread->stat = SMC_THREAD_READY;

	/* delete thread from the any queue */
	smc_list_del_entry(&thread->rlist);

	/**
	 * remove current thread timer from timer list
	 */
	smc_timer_disable(&thread->timer);

	/* put thread to end of ready queue */
	smc_list_add(&thread->rlist,
	             &(smc_list_head_table[thread->priority]));

	smc_bitmap_set(thread->priority);
	smc_cpu_enable_interrupt(status);

	smc_scheduler();

	return SMC_OK;
}


