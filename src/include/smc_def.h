/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-02
 * Describe: all define and structure is here
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_DEF_H
#define SMC_DEF_H

/* include smc_config header to import configuration */
#include "smc_config.h"
#include "smc_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/* define NULL if not defined*/
#ifndef NULL
#ifdef __cplusplus
#define NULL                             0
#else
#define NULL                             ((void *)0)
#endif
#endif

/* SMC-RTOS version information */
#define SMC_VERSION                      0L              /* major version number */
#define SMC_SUBVERSION                   1L              /* minor version number */
#define SMC_REVISION                     1L              /* revise version number */

/* SMC-RTOS basic data type definitions */
typedef signed   char                   smc_int8_t;      /*  8bit integer type */
typedef signed   short                  smc_int16_t;     /* 16bit integer type */
typedef signed   int                    smc_int32_t;     /* 32bit integer type */
typedef unsigned char                   smc_uint8_t;     /*  8bit unsigned integer type */
typedef unsigned short                  smc_uint16_t;    /* 16bit unsigned integer type */
typedef unsigned int                    smc_uint32_t;    /* 32bit unsigned integer type */
typedef int                             smc_bool_t;      /* boolean type */

/* 32bit CPU */
typedef smc_uint32_t                    smc_stack_t;     /* cpu stack type */

/* IO memory access ops */
#define smc_mem_write_32(addr, v)       (*(volatile smc_uint32_t *)(addr) = (v))
#define smc_mem_write_16(addr, v)       (*(volatile smc_uint16_t *)(addr) = (v))
#define smc_mem_write_8(addr, v)        (*(volatile  smc_uint8_t *)(addr) = (v))

#define smc_mem_read_32(addr)           (*(volatile smc_uint32_t *)(addr))
#define smc_mem_read_16(addr)           (*(volatile smc_uint16_t *)(addr))
#define smc_mem_read_8(addr)            (*(volatile  smc_uint8_t *)(addr))

/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
	#include <stdarg.h>
	#define SMC_SECTION(x)              __attribute__((section(x)))
	#define SMC_UNUSED                  __attribute__((unused))
	#define SMC_USED                    __attribute__((used))
	#define SMC_WEAK                    __weak
	#define smc_inline                   static __inline
#else
	#error not supported tool chain
#endif

#define container_of(ptr, type, member) \
	(type *)((char *)(ptr) - (char *) &((type *)0)->member)

/**
 * @def SMC_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. SMC_ALIGN(11, 4)
 * will return 12.
 */
#define SMC_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @def SMC_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. SMC_ALIGN_DOWN(11, 4)
 * will return 8.
 */
#define SMC_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/* error enum */
enum smc_error_e {
	SMC_OK,                                       /* There is no error */
	SMC_ERROR,                                    /* A generic error happens */
	SMC_TIMEOUT,                                  /* Timed out */
	SMC_NOMEM,                                    /* No memory */
	SMC_BUSY                                      /* Busy */
};

/* timer operation mode enum */
enum smc_timer_operation_e {
	SMC_TIMER_DISABLE,                            /* timer disable */
	SMC_TIMER_ONCE,                               /* timer only once */
	SMC_TIMER_PERIODIC,                           /* cycle timer */
	SMC_TIMER_PERIODIC_REINSERT,                  /* timer should re-insert */
};

/* timer set command enum */
enum smc_timer_command_e {
	SMC_TIMER_SET_TIMEOUT_TICK_AFTER,             /* The next timing cycle works */
	SMC_TIMER_SET_TIMEOUT_TICK_IMMEDIATELY,       /* Immediately works */
	SMC_TIMER_SET_OPERATION_MODE,
};

/* thread state enum */
enum smc_thread_state_e {
	SMC_THREAD_INIT,                              /* Initialized status */
	SMC_THREAD_READY,                             /* Ready status */
	SMC_THREAD_SUSPEND,                           /* Suspend status */
	SMC_THREAD_DELETE                             /* Delete status */
};

/**
 * Timer structure
 */
typedef struct smc_timer {
	void (*timerout)(void *parameter);
	void *parameter;
	smc_list_node_t tlist;                         /* thread delay list node */
	smc_uint32_t    init_tick;                     /* init tick */
	smc_uint32_t    timeout_tick;                  /* timeout tick */
	smc_uint8_t     flag;
} smc_timer_t;

/**
 * Thread structure
 */
typedef struct smc_thread {
	/* stack point and entry */
	void            *sp;                           /* stack point */
	smc_uint8_t     priority;                      /* thread priotity */
	smc_uint8_t     stat;                          /* thread state */

	smc_list_node_t rlist;                         /* thread ready list node */

	smc_timer_t     timer;
	smc_uint32_t    init_slice_tick;               /* init slice tick */
	smc_uint32_t    remaining_slice_tick;          /* remaining slice tick */

	smc_int32_t     error_num;                     /* error number */
} smc_thread_t;

#ifdef SMC_USING_SEMAPHORE
/**
 * Semaphore structure
 */
typedef struct smc_sem {
	smc_list_head_t slist;                        /* Thread that is suspended for waiting for a semaphore */
	smc_uint16_t    value;                        /* semaphore value */
} smc_sem_t;
#endif

#ifdef __cplusplus
}
#endif

#endif // SMC_DEF_H
