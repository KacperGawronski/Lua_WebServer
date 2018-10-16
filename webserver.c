/*
This file is part of Extensionable Webserver.

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


#ifdef LUA_ENABLE
#undef LUA_ENABLE
#endif

#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "stack.h"
#include "worker.h"


#ifdef LUA_ENABLE
	#include <lua5.3/lua.h>
	#include <lua5.3/lualib.h>
	#include <lua5.3/lauxlib.h>
#endif

#define MAX_THREADS_NUMBER 2000

sem_t counter_sem;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

int main(void){
	struct addrinfo hints;
	struct addrinfo *result,*rp;
	int s,tmp_s;
	struct sockaddr tmp_sockaddr;
	struct stack_element *tmp;
	socklen_t sockaddr_len;


	for(s=1;s<MAX_THREADS_NUMBER;++s){
		tmp=malloc(sizeof(*tmp));
		tmp->s=0;
		
		#ifdef LUA_ENABLE
			/*STARTING Lua interpreter*/
			tmp->Lua_interpreter=luaL_newstate();
			luaL_openlibs(tmp->Lua_interpreter);
			/*loading app*/
			luaL_dofile(tmp->Lua_interpreter,"app.lua");
		#endif
				
		stack_push(tmp);
	}


	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if(getaddrinfo(NULL,"9090",&hints,&result)==-1){;
		return 1;
	}
	for(rp=result;rp!=NULL;rp=rp->ai_next){
		if((s=socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol))==-1){perror("Socket");continue;}

		if(!bind(s,rp->ai_addr,rp->ai_addrlen))break;
		perror("Bind");
		close(s);/*wykona się tylko w przypadku failure bind()*/
		
	}

	if(rp==NULL){
		freeaddrinfo(result);
		return 2;
	}
	freeaddrinfo(result);
	
	if(listen(s,MAX_THREADS_NUMBER))return 5;


	if(sem_init(&counter_sem,0,MAX_THREADS_NUMBER)==-1){
		return 3;
	}
	
	
	
	while(1){
		sockaddr_len=sizeof(tmp_sockaddr);
		memset(&tmp_sockaddr,0,sockaddr_len);
		if((tmp_s=accept(s,&tmp_sockaddr,&sockaddr_len))<0)return 4;
		tmp=stack_pop();
		tmp->s=tmp_s;
		sem_wait(&counter_sem);
		
		pthread_create(&(tmp->thread_id),NULL,worker,tmp);
	}
}
