#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include "thread_safe_queue.hpp"

class A
{
public:
	int _a;
	A():_a(-1)
	{}
	A(int a):_a(a)
	{}
	~A()
	{
		//printf("destroied %d\n",_a);
	}
};


std::unique_ptr<A> test_ptr()
{
	std::unique_ptr<A> aptr(new A);
	std::unique_ptr<A> bptr(std::move(aptr));
	return aptr;
}

thread_safe_queue<A> que;

void functor1()
{
	for(int i=0;i<10000;i++)
	{
		que.push(A(i));
	}
}

void functor2()
{
	while(true)
	{
		std::shared_ptr<A> a=std::move(que.pop_and_wait());
		printf("%d\n",a->_a);
	}
}

int main()
{
	thread_safe_queue<A> que1;
	for(int i=0;i<10000;i++)
	{
		que1.push(A(i));
		std::shared_ptr<A> a=que1.pop_and_wait();
		printf("%d\n",a->_a);
	}
//	try
//	{
//		std::thread th1(functor1);
//		std::thread th2(functor2);
//		th1.join();
//		th2.join();
//	}
//	catch(std::system_error &e)
//	{
//		printf("catched:%s",e.what());
//	}
//	return 0;
}
