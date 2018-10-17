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



#ifndef STACK_H
#define STACK_H
#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

struct stack_element{
	pthread_t thread_id;
	int s;
	struct stack_element *next,*prev;
	lua_State *Lua_interpreter;
}*stack;


void stack_push(struct stack_element *obj);
struct stack_element *stack_pop(void);
void clear_stack();
#endif
