/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-09
 * Describe: The implementation of doubly linked list for SMC-RTOS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_list.h"

/**
 * [smc_list_for_each_process]
 * the function call node_process_f() for each node from the head, if
 * you want early termination for the specified node, you can make
 * node_process_f() return -1, otherwise return 0.
 *
 * @param  head           [the head for your list]
 * @param  node_process_f [the function for each node will be call]
 * @param  parameter      [the function's parameter]
 *
 * @return                [0 if all node to call node_process_f(), -1 if not]
 */
int smc_list_for_each_process(smc_list_head_t *head,
                              process_func_t node_process_f,
                              void *parameter)
{
	int ret = 0;
	smc_list_node_t *pos = head->next;

	while (pos != head) {
		ret = node_process_f(pos, parameter);
		if (ret < 0)
			break;
		pos = pos->next;
	}

	return ret;
}
