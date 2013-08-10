#include <event2/event.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

void do_accept(evutil_socket_t listener,short event,void* arg)
{
	printf("hello\n");
	accept(listener,NULL,NULL);
}


void init()
{
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(in_addr));
	uint16_t len=sizeof(addr);
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=INADDR_ANY;
	addr.sin_port=htons(9986);
	evutil_socket_t sock=socket(AF_INET,SOCK_STREAM,0);
	evutil_make_listen_socket_reuseable(sock);
	evutil_make_socket_nonblocking(sock);
	bind(sock,(struct sockaddr*)&addr,len);
	listen(sock,3);
	struct event_base *ebase;
	ebase=event_base_new();
	struct event *lisetener_event;
	lisetener_event=event_new(ebase,sock,EV_READ|EV_PERSIST,do_accept,NULL);
	event_add(lisetener_event,NULL);
	event_base_dispatch(ebase);
	event_base_free(ebase);
}

int main()
{
	init();
}
