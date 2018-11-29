/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-03
 * Describe: thread
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_THREAD_H
#define SMC_THREAD_H

#include "smc_def.h"
#include "smc_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern smc_thread_t *smc_thread_current;                      /* point to current thread structure */
extern smc_thread_t *smc_thread_ready;                        /* point to highest priority thread structure */
extern smc_list_head_t smc_list_head_table[SMC_PRIORITY_MAX];   /* ready thread header node for each priority */

/**
 * This function will choose a thread with highest ready priority, and wittch to it run.
 */
void smc_rtos_startup(void);

/**
 * This function will context switch.
 */
void smc_thread_switch(void);

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
                     smc_uint32_t slice_tick);

/**
 * This function will let current thread abandon processor, and scheduler will
 * choose a highest thread to run. After abandon processor, the current thread
 * is still in READY queue.
 */
void smc_thread_abandon(void);

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it.
 */
void smc_scheduler(void);

/**
 * This function will let current thread delay for some ticks.
 *
 * @param delay_tick [the delay ticks]
 */
void smc_thread_delay(smc_uint32_t delay_tick);

/**
 * This function will lock the thread scheduler.
 */
void smc_scheduler_lock(void);

/**
 * This function will unlock the thread scheduler.
 */
void smc_scheduler_unlock(void);

/**
 * This function wiil return the thread with the highest priority level.
 *
 * @return  [thread of the highest priority]
 */
smc_thread_t *smc_thread_highest_ready(void);

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
smc_int32_t smc_thread_suspend(smc_thread_t *thread);

/**
 * This function will resume a thread and put it to system ready queue.
 *
 * @param thread [the thread to be resumed]
 *
 * @return       [the operation status, SMC_OK on OK, -SMC_ERROR on error]
 */
smc_int32_t smc_thread_resume(smc_thread_t *thread);

/**
 * @ingroup Hook
 * This function sets a hook function to idle thread loop. When the system performs
 * idle loop, this hook function should be invoked.
 *
 * @param hook [the specified hook function]
 *
 * @note       [the hook function must be simple and never be blocked or suspend.]
 */
void smc_thread_idle_sethook(void (*hook)(void));

/**
 * Idle thread init
 */
void smc_idle_thread_init(void);

#ifdef __cplusplus
}
#endif

#endif // SMC_THREAD_H
