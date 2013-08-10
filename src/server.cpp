#include <event2/event.h>
#include <stdint.h>
#include <string.h>
#include <event2/event.h>
#include "../hdr/threadpool.hpp"


#define SOCKET_CREAT_ERROR 1
#define ADDRESS_ERROR 2
#define SERVER_BIND_ERROR 3
#define SERVER_LISTEN_ERROR 4
#define THREAD_POOL_ERROR 5
class Server
{
	enum
	{
		CREATED=0,
		INITED=1,
		STARTED=2,
		DEROTY=3,
		STOPPED=4
	};
public:
	Server(const char* server_addr,uint16_t port,uint16_t min_th,uint16_t max_th);
	int32_t init();
	static void service_loop();

private:
	Server(const Server&);
	Server& operator=(const Server&);

	evutil_socket_t _sock;
	char _server_addr[20];
	uint16_t _port;
	thread_pool _pool;
};

static void new_event_func(int fd,short ev,void* arg)
{
	printf("new_event_func received");
	uint32_t newfd;
	int len=read(fd,&newfd,sizeof(newfd));
}

struct service_loop
{
		void operator()(int read_pair)
		{
			struct event_base *base=event_base_new();
			struct event* pair_event=event_new(base,read_pair,EV_READ|EV_PERSIST,);
			event_add(pair_event,NULL);
		}
};

static uint32_t string_to_addr(const char* a,uint32_t len)
{
	bool zeroflag=false;
	uint16_t count=0;
	uint32_t addr=0;
	uint32_t temp=0;
	for(uint32_t i=0;i<len;i++)
	{
		if(a[i]<='9'&&a[i]>='0')
		{
			if(a[i]=='0'&&temp==0)
			{
				if(zeroflag)
					return 0;
				zeroflag=true;
			}
			temp= temp*10+(a[i]-'0');
			if(temp>=255)
				return 0;
		}
		else if(a[i]=='.')
		{
			zeroflag=false;
			if(count>=3)
				return 0;
			addr|=temp<<(8*count);
			temp=0;
			count++;
		}
		else
		{
			return 0;
		}
	}
	addr|=temp<<(8*count);
	return addr;
}



Server::Server(const char* server_addr,uint16_t port,uint16_t min_th,uint16_t max_th)
		:_sock(-1),_port(port),_pool(min_th,max_th)
{
	strcpy(_server_addr,server_addr);
}

int32_t Server::init()
{
	sockaddr_in addr;
	uint32_t serv_addr;

	serv_addr=string_to_addr(_server_addr,strlen(_server_addr));
	if(serv_addr==0)
		return ADDRESS_ERROR;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(_port);
	addr.sin_addr.s_addr=serv_addr;

	_sock=socket(AF_INET,SOCK_STREAM,0);
	if(-1==_sock)
	{
		perror("sock creat error");
		return SOCKET_CREAT_ERROR;
	}

//	if(-1==evutil_make_socket_nonblocking(_sock))
//	{
//		perror("make nonblocking");
//		return SOCKET_CREAT_ERROR;
//	}

	if(-1==evutil_make_listen_socket_reuseable(_sock))
	{
		perror("make reuseable:");
		return SOCKET_CREAT_ERROR;
	}

	try
	{
		_pool.init();
	}
	catch(std::system_error& e)
	{
		printf("init:thread_pool%s",e.what());
		return THREAD_POOL_ERROR;
	}



	if(-1==bind(_sock,(struct sockaddr*)&addr,sizeof(addr)))
	{
		perror("bind:");
		return SERVER_BIND_ERROR;
	}

	if(-1==listen(_sock,10000))
	{
		perror("listen:");
		return SERVER_LISTEN_ERROR;
	}

	while(true)
	{
		if(-1==accept(_sock,NULL,NULL))
		{
			perror("accept:");
			return SERVER_LISTEN_ERROR;
		}
	}
	return 0;
}


int main()
{
	Server server("127.0.0.1",9910,5,10);
	printf("started\n");
	int32_t ret=server.init();
	if(0!=ret)
	{
		printf("%d\n",ret);
		perror("");
	}
	printf("yes\n");
}

