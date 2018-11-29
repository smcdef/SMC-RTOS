/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-17
 * Describe: Init hardware for the special board
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <ARMCM4_FP.h>
#include "smc_rtos.h"

/**
 * This function will init system clock for SMC-RTOS tick
 */
static void systick_init(void)
{
	SysTick_Config(SystemCoreClock / SMC_TICKS_PER_SECOND);
}

/**
 * systick handler
 */
void SysTick_Handler(void)
{
	/* enter interrupt */
	smc_enter_interrupt();

	smc_time_tick();

	/* exit interrupt */
	smc_exit_interrupt();
}

/**
 * This function will init hardware for the special board
 */
void smc_hw_board_init(void)
{
	systick_init();
}
