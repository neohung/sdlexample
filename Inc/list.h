#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>   //free
#include <string.h>   //malloc
#include <stdint.h>   //uintXX_t, intXX_t

typedef uint8_t		u8;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef int8_t		i8;
typedef int32_t		i32;
typedef int64_t		i64;

/*
Returns the number of elements in the given list
*/
#define list_size(list) ((list)->size)

/*
Get the header or tail of the given list
*/
#define list_head(list) ((list)->head)
#define list_tail(list) ((list)->tail)

/*
Access the data for the given element
*/
#define list_data(element) ((element)->data)
#define list_next(element) ((element)->next)
#define list_prev(element) ((element)->prev)

typedef struct ListElement_ {
	void *data;
	struct ListElement_ *prev;
	struct ListElement_ *next;
} ListElement;

typedef struct {
	u32 size;

	void (*destroy)(void *data);

	ListElement *head;
	ListElement *tail;
} List;


List * list_new(void (*destroy)(void* data));
bool list_insert_after(List *list, ListElement *element, void *data);
ListElement * list_item_at(List *list, u32 index);
void * list_remove(List *list, ListElement *element) ;
void list_remove_element_with_data(List *list, void *data);
ListElement * list_search(List *list, void * data);
void list_destroy(List *list);
#endif
