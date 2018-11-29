/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-13
 * Describe: This is a part of SMC-RTOS for timer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_timer.h"
#include "smc_list.h"
#include "smc_cpu.h"
#include "smc_core.h"

static smc_list_head_t smc_timer_resume_list =
	LIST_NODE_INIT(smc_timer_resume_list);                /* Timer need to resume  list */

static smc_list_head_t smc_timer_list =
	LIST_NODE_INIT(smc_timer_list);                       /* thread for all need to delay list head */

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
                    smc_uint8_t flag)
{
	timer->timerout     = timerout;
	timer->parameter    = parameter;
	timer->init_tick    = tick;
	timer->timeout_tick = tick;
	timer->flag         = flag;
	smc_list_node_init(&timer->tlist);
}

/**
 * This function will insert a timer to list
 *
 * @param timer_insert [the timer will be inserted]
 * @param tick         [the tick for delay]
 */
static void smc_timer_insert_list(smc_timer_t *timer_insert, smc_uint32_t tick)
{
	smc_list_node_t *pos;        /* the position where the timer will be inserted */
	smc_uint32_t tick_sum = 0;
	smc_uint32_t timer_tick = 0;
	smc_int32_t status;

	/* disable interrupt */
	status = smc_cpu_disable_interrupt();
	/* find the position where the timer will be inserted */
	for (pos = smc_timer_list.next; pos != &smc_timer_list; pos = pos->next) {
		smc_timer_t *timer = smc_list_entry(pos, smc_timer_t, tlist);
		tick_sum += timer->init_tick;

		/* find the position where the timer will be inserted */
		if (tick_sum >= tick) {
			timer_tick = timer->init_tick;
			timer->init_tick -= tick + timer_tick - tick_sum;
			break;
		}
	}

	/**
	 * if the delay time of the inserted timer is the longest and timer_tick
	 * will be 0
	 */
	timer_insert->init_tick = tick + timer_tick - tick_sum;

	/* put timer to tail of timer delay queue */
	smc_list_add_tail(&timer_insert->tlist, pos);

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);
}

/**
 * This function will enable the special timer
 *
 * @param timer [the timer will be enable]
 */
void smc_timer_enable(smc_timer_t *timer)
{
	if (timer->flag != SMC_TIMER_DISABLE)
		smc_timer_insert_list(timer, timer->timeout_tick);
}

/**
 * This function will disable the special timer
 *
 * @param timer [the timer will be disable]
 */
void smc_timer_disable(smc_timer_t *timer_del)
{
	smc_list_node_t *pos = &timer_del->tlist;
	smc_int32_t status;

	if (timer_del->flag == SMC_TIMER_DISABLE)
		return;

	/* disable interrupt */
	status = smc_cpu_disable_interrupt();

	if (timer_del->flag != SMC_TIMER_PERIODIC_REINSERT) {
		/* if timer is not the last one in the list, should set the next timer tick */
		if (pos->next != &smc_timer_list) {
			smc_timer_t *timer = smc_list_entry(pos->next, smc_timer_t, tlist);

			timer->init_tick += timer_del->init_tick;
		}
	}
	timer_del->flag = SMC_TIMER_DISABLE;

	smc_list_del_entry(&timer_del->tlist);

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);
}

/**
 * The function will set different parameters according to different commands
 *
 * @param timer [the timer to be set]
 * @param cmd   [the command]
 * @param arg   [the argument]
 */
void smc_timer_command(smc_timer_t *timer, smc_uint8_t cmd, void *arg)
{
	smc_int32_t status;

	/* disable interrupt */
	status = smc_cpu_disable_interrupt();

	switch (cmd) {
	case SMC_TIMER_SET_TIMEOUT_TICK_AFTER:
		timer->timeout_tick = *(smc_uint32_t *)arg;
		break;
	case SMC_TIMER_SET_TIMEOUT_TICK_IMMEDIATELY:
		timer->init_tick    = *(smc_uint32_t *)arg;
		timer->timeout_tick = *(smc_uint32_t *)arg;
		break;
	case SMC_TIMER_SET_OPERATION_MODE:
		timer->flag = *(smc_uint8_t *)arg;
		break;
	default:
		break;
	}

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);
}

/**
 * The function will process the special timer according to its flag
 *
 * @param timer [the timer to be process]
 */
static void smc_timer_process(smc_timer_t *timer)
{
	switch (timer->flag) {
	case SMC_TIMER_ONCE:
		smc_list_del_entry(&timer->tlist);
		timer->flag = SMC_TIMER_DISABLE;
		break;
	case SMC_TIMER_PERIODIC:
		smc_list_del_entry(&timer->tlist);
		smc_list_add(&timer->tlist, &smc_timer_resume_list);
		timer->flag = SMC_TIMER_PERIODIC_REINSERT;
		break;
	case SMC_TIMER_PERIODIC_REINSERT:
		timer->flag = SMC_TIMER_PERIODIC;
		break;
	default:
		break;
	}
}

/**
 * The function will make a timer counter decrease, and should be invoked
 * in interrupt handle.
 */
void smc_timer_decrease(void)
{
	smc_timer_t *timer;
	smc_list_node_t *pos = smc_timer_list.next;
	smc_uint32_t status = smc_cpu_disable_interrupt();

	if (smc_list_is_empty(&smc_timer_list)) {
		smc_cpu_enable_interrupt(status);
		return;
	}

	timer = smc_list_entry(pos, smc_timer_t, tlist);
	timer->init_tick--;

	/* put all thread of that delay tick is 0 to ready queue */
	while ((timer->init_tick == 0) && (pos != &smc_timer_list)) {
		pos = pos->next;
		smc_timer_process(timer);
		timer->timerout(timer->parameter);
		timer = smc_list_entry(pos, smc_timer_t, tlist);
	}

	/* If there is a periodic timer, re-insert it into the timer delay list */
	if (!smc_list_is_empty(&smc_timer_resume_list)) {
		smc_list_node_t *pos = smc_timer_resume_list.next;
		smc_timer_t *timer;

		while (pos != &smc_timer_resume_list) {
			pos = pos->next;
			timer = smc_list_entry(pos->prev, smc_timer_t, tlist);
			smc_timer_insert_list(timer, timer->timeout_tick);
			smc_timer_process(timer);
		}
		smc_list_node_init(&smc_timer_resume_list);
	}

	/* enable interrupt */
	smc_cpu_enable_interrupt(status);

	/* do scheduler */
	smc_scheduler();
}
