#include "list.h"

#include <OS.h>


void list_init(list_t **l)
{
	(*l)=0;
}


void list_prepend(list_t **l, void *data)
{
	list_t *item=(list_t*)heap_malloc(sizeof(list_t));
	item->data=data;
	item->next=(*l);
	(*l)=item;
}


void list_append(list_t **l, void *data)
{
	while (*l) l=&((*l)->next);
	(*l)=(list_t*)heap_malloc(sizeof(list_t));
	(*l)->data=data;
	(*l)->next=NULL;
}


void list_remove(list_t **l, void *data)
{
	while (*l)
	{
		if ((*l)->data == data)
		{
			list_t *del=(*l);
			(*l)=(*l)->next;
			heap_free((void*)del);
			return;
		}
		l=&((*l)->next);
	}
}


bool list_contains(list_t **l, void *data)
{
	while (*l)
	{
		if ((*l)->data == data) return true;
		l=&((*l)->next);
	}
	return false;
}


bool list_empty(list_t **l)
{
	return (*l) == NULL;
}


void* list_take_first(list_t **l)
{
	if (! *l) return NULL;
	
	void *ret=(*l)->data;
	list_t *del=(*l);
	
	(*l)=(*l)->next;
	
	heap_free((void*)del);
	
	return ret;
}


void* list_first(list_t **l)
{
	if (*l)
		return (*l)->data; else
		return NULL;
}


void* list_last(list_t **l)
{
	while ( (*l) && ((*l)->next) )
		l=&((*l)->next);
	
	if (*l)
		return (*l)->data; else
		return NULL;
}


void* list_at(list_t **l, uint32_t index)
{
	while ( (index > 0) && (*l) )
	{
		index--;
		l=&((*l)->next);
	}
	
	if ( (index == 0) && (*l) )
		return (*l)->data; else
		return NULL;
}
