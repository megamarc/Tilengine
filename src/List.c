/* generic, array-based double linked list */

#include <stdint.h>
#include <stddef.h>
#include "List.h"
#include "Debug.h"

static ListNode* get_node(List* list, int node)
{
	if (node != -1)
	{
		uint8_t* addr = (uint8_t*)list->base;
		return (ListNode*)(addr + node * (list->node_size));
	}
	return NULL;
}

void ListInit(List* list, ListNode* base, int node_size, int num_nodes)
{
	int c;
	list->base = (void*)base;
	list->node_size = node_size;
	list->num_nodes = num_nodes;
	list->first = -1;
	list->last = -1;

	for (c = 0; c < num_nodes; c += 1)
	{
		ListNode* node = get_node(list, c);
		node->prev = -1;
		node->next = -1;
	}
}

void ListLinkNodes(List* list, int num1, int num2)
{
	ListNode* node;

	node = get_node(list, num1);
	if (node)
		node->next = num2;

	node = get_node(list, num2);
	if (node)
		node->prev = num1;
}

void ListUnlinkNode(List* list, int num)
{
	ListNode* node = get_node(list, num);
	ListNode* node_prev = get_node(list, node->prev);
	ListNode* node_next = get_node(list, node->next);

	if (node_prev)
		node_prev->next = node->next;
	if (node_next)
		node_next->prev = node->prev;
	if (list->first == num)
		list->first = node->next;
	if (list->last == num)
		list->last = node->prev;
	node->prev = -1;
	node->next = -1;
	ListPrint(list);
}

void ListAppendNode(List* list, int num)
{
	if (list->first == -1)
		list->first = num;
	ListLinkNodes(list, list->last, num);
	list->last = num;
	ListPrint(list);
}

int ListGetPrev(List* list, int num)
{
	ListNode* node = get_node(list, num);
	return node->prev;
}

int ListGetNext(List* list, int num)
{
	ListNode* node = get_node(list, num);
	return node->next;
}

void ListPrint(List* list)
{
#ifdef _DEBUG
	int index;
	int c = 0;
	debugmsg("list: ");
	index = list->first;
	while (index != -1)
	{
		ListNode *node = get_node(list, index);
		debugmsg("%d ", index);
		index = node->next;
		c += 1;
		if (c > list->num_nodes)
		{
			exit(0);
		}
	}
	debugmsg("\n");
#endif
}
