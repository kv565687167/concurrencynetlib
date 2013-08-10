/*
 * thread_safe_queue.hpp
 *
 *  Created on: 2013年7月31日
 *      Author: yewenchao
 */

#ifndef THREAD_SAFE_QUEUE_HPP_
#define THREAD_SAFE_QUEUE_HPP_
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T> class thread_safe_queue
{
private:
	typedef std::unique_ptr<T> data;
	struct que_node
	{
		std::shared_ptr<T> this_data;
		std::unique_ptr<que_node> next;
	};

private:
	std::unique_ptr<que_node> head;
	que_node* tail;
	std::mutex head_mutex,tail_mutex;
	std::condition_variable cond;

	que_node* get_tail()
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}
public:
	thread_safe_queue():
		head(new que_node),tail(head.get())
	{
	}

	std::shared_ptr<T> pop_and_wait()
	{
		std::unique_lock<std::mutex> head_lock(head_mutex);
		{
			cond.wait(head_lock,[this](){return this->head.get()!=this->get_tail();});
		}
		printf("abc");
		std::shared_ptr<T> data=head->this_data;
		head=std::move(head->next);
		return data;
	}

	void push(T t)
	{
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(t)));
		std::unique_ptr<que_node> new_tail(new que_node);
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			tail->next=std::move(new_tail);
			tail->this_data=new_data;
			tail=tail->next.get();
		}
	}
private:
	thread_safe_queue(thread_safe_queue &que)=delete;
	thread_safe_queue& operator=(thread_safe_queue)=delete;
};

template<typename T> class threadsafe_queue2
{
private:
	std::queue<T> que;
};



#endif /* THREAD_SAFE_QUEUE_HPP_ */
