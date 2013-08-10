#include <event2/event.h>
#include <event2/bufferevent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void readcb(struct bufferevent* buff,void* arg)
{
	printf("begin read\n");
	char* a=(char*)malloc(50000);
	bufferevent_read(buff,(void*)a,50000);
	printf("receive 50000\n");
}

static void writecb(struct bufferevent* buff,void* arg)
{
}

static void eventcb(bufferevent* buff,short what,void* arg)
{
	if(what & BEV_EVENT_CONNECTED)
	{
		printf("connected \n");
		bufferevent_enable(buff,EV_WRITE|EV_READ);
		bufferevent_write(buff,"abcd",4);
	}
}

void init_client()
{
	struct event_base* base;
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=htonl(0x7f000001);
	sin.sin_port=htons(5555);

	base=event_base_new();
	if(base==NULL)
	{
		printf("error base");
		return;
	}
	bufferevent* bev[1];
	for(int i=0;i<1;i++)
	{
		bev[i]=bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(bev[i],readcb,writecb,eventcb,(void*)base);
		bufferevent_socket_connect(bev[i],(struct sockaddr*)&sin,sizeof(sin));
	}
	event_base_dispatch(base);
}

int main()
{
	init_client();
}
