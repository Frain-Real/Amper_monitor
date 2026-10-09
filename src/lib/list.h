#ifndef LIB_LIST_H
#define LIB_LIST_H


#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct list_s
{
	struct list_s *next;
	void    *data;
} list_t;


void list_init(list_t **l);
void list_prepend(list_t **l, void *data);
void list_append(list_t **l, void *data);
void list_remove(list_t **l, void *data);
bool list_contains(list_t **l, void *data);
bool list_empty(list_t **l);
void* list_take_first(list_t **l);
void* list_first(list_t **l);
void* list_last(list_t **l);
void* list_at(list_t **l, uint32_t index);


#ifdef __cplusplus
}
#endif


#endif
