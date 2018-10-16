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
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#define MAX_THREADS_NUMBER 2000

sem_t counter_sem;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

lua_State *Lua_interpreter;

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
	
	
	/*STARTING Lua interpreter*/
	Lua_interpreter=luaL_newstate();
	luaL_openlibs(Lua_interpreter);
	/*loading app*/
	luaL_dofile(Lua_interpreter,"app.lua");
	
	
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
