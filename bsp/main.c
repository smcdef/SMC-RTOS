/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-02
 * Describe: startup
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_rtos.h"

void smc_hw_board_init(void);
void smc_app_init(void);

/**
 * This function will startup SMC-RTOS
 */
void smc_rtos_startup(void)
{
	/* init board */
	smc_hw_board_init();

	/* init SMC-RTOS */
	smc_rtos_init();

	/* idle thread init */
	smc_idle_thread_init();

	/* user application init */
	smc_app_init();

	/* start scheduler */
	smc_rtos_scheduler();
}

int main(void)
{
	/* disable system interrupt */
	smc_cpu_disable_interrupt();

	/* SMC-RTOS start */
	smc_rtos_startup();

	return 0;
}
