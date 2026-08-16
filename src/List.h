#ifndef _LIST_H
#define _LIST_H

typedef struct
{
	int prev;		/* index of prev node */
	int next;		/* index of next node */
}
ListNode;

typedef struct
{
	void* base;		/* ptr to first "ListNode" item */
	int node_size;	/* size of nodes */
	int num_nodes;	/* total number of nodes in array */
	int first;		/* index to first node */
	int last;		/* index to last node */
}
List;

void ListInit(List* list, ListNode* base, int node_size, int num_nodes);
void ListLinkNodes(List* list, int num1, int num2);
void ListUnlinkNode(List* list, int node);
void ListAppendNode(List* list, int node);
void ListPrint(List* list);
int ListGetPrev(List* list, int num);
int ListGetNext(List* list, int num);

#endif
