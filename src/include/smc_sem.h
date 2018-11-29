/**
 * Author:   songmuchun <smcdef@163.com>
 * Date:     2017-06-12
 * Describe: This is a part of SMC-RTOS for semaphore
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_SEM_H
#define SMC_SEM_H

#include "smc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SMC_USING_SEMAPHORE

/**
 * semaphore wait mode
 */
#define SMC_SEM_WAIT_FOREVER            -1
#define SMC_SEM_NO_WAIT                  0

/**
 * This function will initialize a semaphore
 *
 * @param sem   [the semaphore]
 * @param value [the init value of semaphore]
 */
void smc_sem_init(smc_sem_t *sem, smc_uint16_t value);

/**
 * This function will wait a semaphore, if the semaphore is unavailable, the
 * thread shall wait for a specified time.
 *
 * @param sem      [the semaphore]
 * @param time_out [the waiting time]
 *
 * @return         [error number]
 */
smc_int32_t smc_sem_pend(smc_sem_t *sem, smc_int32_t time_out);

/**
 * This function will release a semaphore, if there are threads suspended on
 * semaphore, it will be waked up.
 *
 * @param sem [the semaphore]
 *
 * @return    [the error number]
 */
smc_int32_t smc_sem_release(smc_sem_t *sem);

#endif /* SMC_USING_SEMAPHORE */

#ifdef __cplusplus
}
#endif

#endif // SMC_SEM_H
