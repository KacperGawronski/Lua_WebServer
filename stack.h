#ifndef STACK_H
#define STACK_H
#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>
struct stack_element{
	pthread_t thread_id;
	int s;
	struct stack_element *next,*prev;
}*stack;


void stack_push(struct stack_element *obj);
struct stack_element *stack_pop(void);
void clear_stack();
#endif
