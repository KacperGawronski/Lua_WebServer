/*This file is part of Extensionable Webserver.

Extensionable Webserver is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Extensionable Webserver is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Extensionable Webserver. If not, see
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

#ifdef LUA_ENABLE
	#include <lua5.3/lua.h>
	#include <lua5.3/lualib.h>
	#include <lua5.3/lauxlib.h>
#endif

#define HTTP_REQUEST_SIZE 10240

void *worker(void *arg){
	extern sem_t counter_sem;
	char buffer[HTTP_REQUEST_SIZE];
	const char *response;
	buffer[HTTP_REQUEST_SIZE-1]='\0';
	int n;
	
	
	n=recv(((struct stack_element *)arg)->s,buffer,HTTP_REQUEST_SIZE,MSG_DONTWAIT);
	#ifdef LUA_ENABLE
		lua_getglobal(((struct stack_element *)arg)->Lua_interpreter,"process_http_request");
		lua_pushstring(((struct stack_element *)arg)->Lua_interpreter,buffer);
		lua_call(((struct stack_element *)arg)->Lua_interpreter,1,1);
		lua_call(((struct stack_element *)arg)->Lua_interpreter,0,1);
		response=lua_tostring(((struct stack_element *)arg)->Lua_interpreter,-1);
	#endif
	#ifndef LUA_ENABLE
		response="HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n<html><body>Hello World</body></html>\r\n";
	#endif
	if(n>0)send(((struct stack_element *)arg)->s,response,strlen(response),0);
	
	
	
	close(((struct stack_element*)arg)->s);
	stack_push((struct stack_element *)arg);
	sem_post(&counter_sem);
	return NULL;
}
#undef MSG
