/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-12
 * Describe: This is a part of SMC-RTOS for semaphore
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_sem.h"
#include "smc_list.h"
#include "smc_thread.h"
#include "smc_core.h"
#include "smc_timer.h"

#ifdef SMC_USING_SEMAPHORE
/**
 * This function will initialize a semaphore
 *
 * @param sem   [the semaphore]
 * @param value [the init value of semaphore]
 */
void smc_sem_init(smc_sem_t *sem, smc_uint16_t value)
{
	smc_list_node_init(&sem->slist);
	sem->value = value;
}

/**
 * This function will wait a semaphore, if the semaphore is unavailable, the
 * thread shall wait for a specified time.
 *
 * @param sem      [the semaphore]
 * @param time_out [the waiting time]
 *
 * @return         [error number]
 */
smc_int32_t smc_sem_pend(smc_sem_t *sem, smc_int32_t time_out)
{
	smc_uint32_t status;

	/* disable interrupt */
	status = smc_cpu_disable_interrupt();

	if (sem->value > 0) {
		sem->value--;

		/* enable interrupt */
		smc_cpu_enable_interrupt(status);
	} else {
		if (time_out == SMC_SEM_NO_WAIT) {
			smc_cpu_enable_interrupt(status);
			return -SMC_TIMEOUT;
		} else {
			/* reset thread error number */
			smc_thread_current->error_num = SMC_OK;

			/* suspend the current thread and do schedule */
			smc_thread_suspend(smc_thread_current);

			if (time_out != SMC_SEM_WAIT_FOREVER) {
				smc_uint8_t flag = SMC_TIMER_ONCE;

				/* set timer timeout tick */
				smc_timer_command(&smc_thread_current->timer,
				                  SMC_TIMER_SET_TIMEOUT_TICK_IMMEDIATELY,
				                  &time_out);

				/* set timer flag */
				smc_timer_command(&smc_thread_current->timer,
				                  SMC_TIMER_SET_OPERATION_MODE,
				                  &flag);

				/* timer startup */
				smc_timer_enable(&smc_thread_current->timer);
			}

			/* add the current thread to semaphore list */
			smc_list_add(&smc_thread_current->rlist, &sem->slist);

			smc_scheduler();

			/* enable interrupt, and will make contex switch */
			smc_cpu_enable_interrupt(status);

			if (smc_thread_current->error_num != SMC_OK)
				return smc_thread_current->error_num;
		}
	}

	return SMC_OK;
}

/**
 * This function will release a semaphore, if there are threads suspended on
 * semaphore, it will be waked up.
 *
 * @param sem [the semaphore]
 *
 * @return    [the error number]
 */
smc_int32_t smc_sem_release(smc_sem_t *sem)
{
	smc_uint32_t status;

	/* disable interrupt */
	status = smc_cpu_disable_interrupt();

	if (!smc_list_is_empty(&sem->slist)) {
		smc_thread_t *thread;

		thread = smc_list_entry(sem->slist.next, smc_thread_t, rlist);

		/**
		 * resume the first thread in the pending thread list waiting for
		 * the special semaphore.
		 */
		smc_thread_resume(thread);
	} else {
		sem->value++;
	}

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);

	return SMC_OK;
}

#endif /* SMC_USING_SEMAPHORE */

