#include "stack.h"
void clear_stack(void){ while(stack)free(stack_pop()); }

struct stack_element *stack_pop(void){
	struct stack_element *tmp;
	if(stack){
		extern pthread_mutex_t mut;
		pthread_mutex_lock(&mut);
		tmp=stack;
		stack=stack->next;
		pthread_mutex_unlock(&mut);
		return tmp;
	}
	return NULL;
}

void stack_push(struct stack_element *obj){
	struct stack_element *tmp;
	extern pthread_mutex_t mut;
	pthread_mutex_lock(&mut);
	tmp=stack;
	stack=obj;
	stack->next=tmp;
	if(tmp)stack->next->prev=stack;
	stack->prev=NULL;
	pthread_mutex_unlock(&mut);
}
