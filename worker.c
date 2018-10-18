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

#include "worker.h"
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include "stack.h"
#include <semaphore.h>
#include <string.h>
#include <stdio.h>

#include <regex.h>

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>


#define HTTP_REQUEST_SIZE 10240

void *worker(void *arg){
	extern sem_t counter_sem;
	char buffer[HTTP_REQUEST_SIZE];
	const char *response;
	buffer[HTTP_REQUEST_SIZE-1]='\0';
	int n,i;
	
	n=recv(((struct stack_element *)arg)->s,buffer,HTTP_REQUEST_SIZE-1,MSG_DONTWAIT);
		if(!strncmp(buffer,"GET",3)){
			
			lua_getglobal(((struct stack_element *)arg)->Lua_interpreter,"process_request");
			lua_pushstring(((struct stack_element *)arg)->Lua_interpreter,buffer);
			lua_call(((struct stack_element *)arg)->Lua_interpreter,1,1);
				
			while((!lua_isnil(((struct stack_element *)arg)->Lua_interpreter,-1))&&LUA_YIELD==lua_resume(((struct stack_element *)arg)->Lua_interpreter,NULL,0)){
				response=lua_tostring(((struct stack_element *)arg)->Lua_interpreter,-1);
				send(((struct stack_element *)arg)->s,response,strlen(response),MSG_DONTWAIT);
				lua_pop(((struct stack_element *)arg)->Lua_interpreter,1);
			}
		}
		
		
	close(((struct stack_element*)arg)->s);
	stack_push((struct stack_element *)arg);
	sem_post(&counter_sem);
	return NULL;
}
