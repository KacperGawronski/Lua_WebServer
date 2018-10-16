#include "worker.h"
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include "stack.h"
#include <semaphore.h>
#include <string.h>
#include <stdio.h>

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#define BUFFER_SIZE 10240

void *worker(void *arg){
	extern sem_t counter_sem;
	extern lua_State *Lua_interpreter;
	char buffer[BUFFER_SIZE];
	const char *response;
	buffer[BUFFER_SIZE-1]='\0';
	int n;
	
	
	n=recv(((struct stack_element *)arg)->s,buffer,BUFFER_SIZE,MSG_DONTWAIT);
	lua_getglobal(Lua_interpreter,"process_http_request");
	lua_pushstring(Lua_interpreter,buffer);
	lua_call(Lua_interpreter,1,1);
	lua_call(Lua_interpreter,0,1);
	response=lua_tostring(Lua_interpreter,-1);

	if(n>0)send(((struct stack_element *)arg)->s,response,strlen(response),0);
	
	
	
	close(((struct stack_element*)arg)->s);
	stack_push((struct stack_element *)arg);
	sem_post(&counter_sem);
	return NULL;
}
#undef MSG
