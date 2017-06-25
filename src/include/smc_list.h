/**
 * Author:   SMC<smcdef@163.com>
 * Date:     2017-06-09
 * Describe: The implementation of doubly linked list for SMC-RTOS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SMC_LIST_H
#define SMC_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
	#define smc_inline                   static __inline
#else
	#define smc_inline
#endif
/* doubly linked list structure */
struct smc_list_node {
	struct smc_list_node *prev;
	struct smc_list_node *next;
};
typedef struct smc_list_node smc_list_node_t;
typedef struct smc_list_node smc_list_head_t;

#define LIST_NODE_INIT(name) { &(name), &(name) }
#define LIST_NODE(name) \
	struct smc_list_node name = LIST_HEAD_INIT(name)

smc_inline void smc_list_node_init(struct smc_list_node *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
smc_inline void __list_add(struct smc_list_node *new,
                           struct smc_list_node *prev,
                           struct smc_list_node *next)
{
	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
smc_inline void smc_list_add(struct smc_list_node *new, struct smc_list_node *head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
smc_inline void smc_list_add_tail(struct smc_list_node *new, struct smc_list_node *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
smc_inline void smc_list_del_entry(struct smc_list_node *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	smc_list_node_init(entry);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
smc_inline int smc_list_is_empty(const struct smc_list_node *node)
{
	return node->next == node;
}

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define smc_list_entry(node, type, member)\
	(type *)((char *)(node) - (char *) &((type *)0)->member)

/**
 * smc_list_for_each_entry  -   iterate over list of given type
 * @type    the type of structure
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define smc_list_for_each_entry(pos, type, head, member)              \
	for (pos = smc_list_entry((head)->next, type, member);  \
	     &pos->member != (head);    \
	     pos = smc_list_entry(pos->member.next, type, member))

/**
 * smc_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define smc_list_first_entry(ptr, type, member) \
	smc_list_entry((ptr)->next, type, member)

typedef int(*process_func_t)(smc_list_node_t *node, void *parameter);
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
                              void *parameter);

#ifdef __cplusplus
}
#endif

#endif // SMC_LIST_H
