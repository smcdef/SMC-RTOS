/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-03
 * Describe: SMC-RTOS for cortex-m3
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_rtos.h"

#define NVIC_INT_CTRL        0xE000ED04
#define NVIC_PENDSVSET       0x10000000
#define NVIC_SYSPRI2         0xE000ED20
#define NVIC_PENDSV_PRI      0xFFFF0000

/**
 * This function will make contex switch
 */
__asm void PendSV_Handler(void)
{
	IMPORT smc_thread_current
	IMPORT smc_thread_ready

	CPSID   I                        /* Prevent interruption during context switch */
	LDR R1, =smc_thread_current
	LDR R1, [R1]
	CBZ R1, PendSV_Handler_Nosave    /* skip save R4-R11 for first run user thread */
	MRS R0, PSP

	STMFD R0!, {R4-R11}
	STR R0, [R1]                     /* smc_thread_current->sp = PSP */

PendSV_Handler_Nosave
	LDR R0, =smc_thread_current      /* smc_thread_current = smc_thread_ready */
	LDR R1, =smc_thread_ready
	LDR R2, [R1]
	STR R2, [R0]

	LDR R3, [R2]
	LDMFD R3!, {R4-R11}
	STR R3, [R2]
	MSR PSP, R3
	ORR LR, LR, #0x04
	CPSIE   I                        /* Prevent interruption during context switch */
	BX LR
	NOP
}

/**
 * This function will initialize thread stack
 *
 * @param tentry     [the entry of thread]
 * @param parameter  [the parameter of entry]
 * @param stack_addr [the beginning stack address]
 *
 * @return stack address
 */
smc_stack_t *smc_thread_stack_init(void (*entry)(void *parameter),
                                   void *parameter,
                                   smc_stack_t *stack_addr)
{
	/* Align the stack to 8-bytes */
	stack_addr = (smc_stack_t *)SMC_ALIGN_DOWN((smc_stack_t)stack_addr, 8);
	
	*(--stack_addr) = (smc_stack_t)(1 << 24);     /* xPSR                                       */
	*(--stack_addr) = (smc_stack_t)entry;         /* R15 (PC) (Entry Point)                     */
	*(--stack_addr) = (smc_stack_t)0;             /* R14 (LR)                                   */
	*(--stack_addr) = (smc_stack_t)0;             /* R12                                        */
	*(--stack_addr) = (smc_stack_t)0;             /* R3                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R2                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R1                                         */
	*(--stack_addr) = (smc_stack_t)parameter;     /* R0 : argument                              */
	                                              /* Remaining registers saved on process stack */
	*(--stack_addr) = (smc_stack_t)0;             /* R11                                        */
	*(--stack_addr) = (smc_stack_t)0;             /* R10                                        */
	*(--stack_addr) = (smc_stack_t)0;             /* R9                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R8                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R7                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R6                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R5                                         */
	*(--stack_addr) = (smc_stack_t)0;             /* R4                                         */

	return stack_addr;
}

/**
 * This function will make context switch.
 *
 * @note [switch not in interrupt]
 *
 */
void smc_thread_switch(void)
{
	smc_mem_write_32(NVIC_INT_CTRL, NVIC_PENDSVSET);
}

/**
 * This function will make context switch.
 *
 * @note [switch in interrupt]
 *
 */
void smc_thread_intrrupt_switch(void)
{
	smc_mem_write_32(NVIC_INT_CTRL, NVIC_PENDSVSET);
}

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it. It only for the
 * first context switch
 */
void smc_thread_switch_to(void)
{
	smc_mem_write_32(NVIC_SYSPRI2, NVIC_PENDSV_PRI);
	smc_mem_write_32(NVIC_INT_CTRL, NVIC_PENDSVSET);
	__asm {
		CPSIE   I
	}
}

/**
 * This function will return current system interrupt status and disable system
 * interrupt.
 *
 * @return [the current system interrupt status]
 */
__asm smc_uint32_t smc_cpu_disable_interrupt(void)
{
	MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}

/**
 * This function will set the specified interrupt status, which shall saved by
 * smc_cpu_disable_interrupt function. If the saved interrupt status is interrupt
 * opened, this function will open system interrupt status.
 */
__asm void smc_cpu_enable_interrupt(smc_uint32_t status)
{
	MSR     PRIMASK, R0
    BX      LR
}

/**
 * This function will delay some microseconds(us).
 *
 * @param us [Delay time]
 */
void smc_cpu_us_delay(smc_uint32_t us)
{
	smc_uint32_t delta;

	/* Get delay timer count */
	us = us * (SysTick->LOAD / (1000000 / SMC_TIMER_PERIODIC));
	
	/* Get current timer count */
	delta = SysTick->VAL;

	while (delta - SysTick->VAL < us);
}
