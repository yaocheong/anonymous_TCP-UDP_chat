#include "common.h"

//验证节点p在链表中是否存在
bool list_for_each(friend_list head, friend_node p)
{
	bool switches = false;
	friend_list n = head->next;
	for (n = head->next; n != NULL; n = n->next)
	{
		//若节点p在list中存在
		if (strcmp(n->name, p.name) == 0)
		{
			switches = true;
			break;
		}
	}

	//若节点p在list中存在
	if (n != NULL)
		switches = true;

	return switches;
}

friend_list new_node(friend_list info)
{
	friend_list new_node;

	new_node = malloc(sizeof(friend_node));
	if (new_node == NULL)
	{
		perror("申请客户端节点异常\n");
		return NULL;
	}

	if (info != NULL)
		*new_node = *info;

	new_node->next = NULL;

	return new_node;
}

void insert_node(friend_list head, friend_list new_node)
{
	friend_list p;

	for (p = head; p->next != NULL; p = p->next);

	p->next = new_node;
}

void delete_node(friend_list head, friend_list node)
{
	friend_list n, p;
	n = head;
	p = n->next;

	while(p->name != node->name && p->next != NULL)
	{
		n = n->next;
		p = n->next;
	}

	if (p->next != NULL)
	{
		n->next = p->next;
		p->next = NULL;
	}
	else if (p->next == NULL)
	{
		n->next = NULL;
	}
	free(p);
}

void destroy_list(friend_list head)
{
	friend_list n, p;

	n = head;
	p = head->next;
	if (p != NULL)
	{
		while(1)
		{
			if (p->next == NULL)
				break;
			n->next = p->next;
			p->next = NULL;
			free(p);
			p = n->next;
		}

		n->next = NULL;
		free(p);
		free(n);
	}
	else if (p == NULL)
	{
		free(n);
	}

	printf("链表已清除完成\n");
}

void display(global ginfo, friend_list pos)
{
	for (pos = ginfo.list_head->next; pos != NULL; pos = pos->next)
	{
		printf("[%s:%d]:", 
				inet_ntoa(pos->addr.sin_addr), 
				ntohs(pos->addr.sin_port));
		printf("%s 在线\n", 
			pos->name);
	}
}