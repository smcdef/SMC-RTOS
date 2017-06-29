/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-17
 * Describe: User application
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_rtos.h"

static smc_uint8_t task1_stack[512];
static smc_thread_t task1_thread;

static smc_uint8_t task2_stack[512];
static smc_thread_t task2_thread;

static smc_uint8_t task3_stack[512];
static smc_thread_t task3_thread;

static smc_timer_t timer1;
static smc_sem_t sem;

int time1_loop;
static void timeout1(void *parameter)
{
	time1_loop = !time1_loop;
	smc_sem_release(&sem);
}

static void delay(unsigned int dly)
{
	while (dly--);
}

int task1_togo;
static void task1_thread_entry(void *param)
{
	while (1) {
		task1_togo = !task1_togo;
		smc_sem_pend(&sem, SMC_SEM_WAIT_FOREVER);
	}
}

int task2_togo;
static void task2_thread_entry(void *param)
{
	while (1) {
		task2_togo = 0;
		smc_thread_delay(4);
		task2_togo = 1;
		smc_thread_delay(4);
	}
}

int task3_togo;
static void task3_thread_entry(void *param)
{
	while (1) {
		task3_togo = 0;
		smc_thread_delay(8);
		task3_togo = 1;
		smc_thread_delay(8);
	}
}

void smc_app_init(void)
{
	smc_thread_init(&task1_thread,
	                task1_thread_entry,
	                NULL,
	                0,
	                task1_stack,
	                sizeof(task1_stack),
	                20);
	smc_thread_init(&task2_thread,
	                task2_thread_entry,
	                NULL,
	                1,
	                task2_stack,
	                sizeof(task2_stack),
	                20);
	smc_thread_init(&task3_thread,
	                task3_thread_entry,
	                NULL,
	                2,
	                task3_stack,
	                sizeof(task3_stack),
	                20);
	smc_timer_init(&timer1,
	               2,
	               timeout1,
	               NULL,
	               SMC_TIMER_PERIODIC);
	smc_timer_enable(&timer1);
	smc_sem_init(&sem, 0);
}
