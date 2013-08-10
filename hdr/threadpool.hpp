/*
 * threadpool.hpp
 *
 *  Created on: 2013年8月8日
 *      Author: yewenchao
 */

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_
#include <thread>
#include <condition_variable>
#include <vector>
#include <list>
#include <functional>
#include <queue>

struct thread_destoryer
{
	void operator()(std::thread* th1)
	{
		if(th1->joinable())
			th1->join();
		delete(th1);
	}
};
class thread_pool
{
//	typedef std::shared_ptr<std::thread,thread_destoryer> thread;
public:
	void worker_thread();
	thread_pool(uint32_t min_th_num,uint32_t max_th_num);
	void init();
	void add_task(std::function<void()>& task);

	thread_pool(const thread_pool&)=delete;
	thread_pool& operator=(const thread_pool&)=delete;
private:
	bool done;
	uint32_t _min_th_num;
	uint32_t _max_th_num;
	uint32_t _cur_th_num;
	uint32_t _idle_th_num;
	std::mutex _mutex;
	std::mutex _num_mutex;
	std::condition_variable _cond;
	std::list<std::shared_ptr<std::thread> > _thread_list;
	std::queue<std::function<void()>,std::vector<std::function<void()> > >_task_que;
};

void thread_pool::add_task(std::function<void()>& task)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_task_que.push(task);
		_cond.notify_one();
	}
	{
		std::lock_guard<std::mutex> lock(_num_mutex);
		if(_idle_th_num==0&&_cur_th_num<_max_th_num)
		{
			_thread_list.push_back(std::shared_ptr<std::thread>(new std::thread(
					&thread_pool::worker_thread,this),thread_destoryer()));
			_cur_th_num++;
			_idle_th_num++;
		}
	}

}

void thread_pool::worker_thread()
{
	while(!done)
	{
		std::function<void()> fun;
		{
			std::unique_lock<std::mutex> lock(_mutex);
			while(_thread_list.empty())
			{
				_cond.wait(lock);
			}
			fun=_task_que.front();
			_task_que.pop();
			lock.unlock();
		}
		fun();
		{
			std::lock_guard<std::mutex> lock(_num_mutex);
			if(_cur_th_num>_min_th_num)
			{
				_cur_th_num--;
				break;
			}
		}
	}

}

thread_pool::thread_pool(uint32_t min_th_num,uint32_t max_th_num):
		done(false),_min_th_num(min_th_num),_max_th_num(max_th_num),_cur_th_num(0),_idle_th_num(0)
{

}

void thread_pool::init()
{
	for(uint32_t i=0;i<_min_th_num;i++)
	{
		_thread_list.push_back(std::shared_ptr<std::thread>(new std::thread(
				&thread_pool::worker_thread,this),thread_destoryer()));
		_cur_th_num++;
		_idle_th_num++;
	}
}



#endif /* THREADPOOL_HPP_ */
