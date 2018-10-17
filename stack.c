/*This file is part of Lua Webserver.

Lua Webserver is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lua Webserver is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lua Webserver. If not, see
https://www.gnu.org/licenses/
*/


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
