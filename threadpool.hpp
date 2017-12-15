#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <string>
#include <queue>


using Task = std::function<void()>;

template<size_t thread_num=10>
class ThreadPool
{
public:
	ThreadPool() 
		:is_running(false)
	{
		for (size_t i = 0; i < thread_num; ++i)
			pool.emplace_back(&ThreadPool::work,this);
	}
	void run()
	{
		is_running.store(true);
	}
	void stop()
	{
		is_running.store(false);
	}
	void append(Task task)
	{
		std::unique_lock<std::mutex> lck(mtx);
		tasks.push(task); 
		cv.notify_one();
	}
	~ThreadPool()
	{

		for (auto& td : pool) // detach all
			td.detach(); 
	}
private:
	// get one task.
	Task get_task() 
	{
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, [this] {return !tasks.empty(); });
		auto task = tasks.front();
		tasks.pop();
		return task;
	}
	// every thread use this function to get one task and do it.
	void work()
	{
		while (true)
		{
			auto task = std::move(get_task());
			if (!task)
				continue;
			task();
		}


	}
private:
	// thread pool
	std::vector<std::thread> pool; 
	// task queue
	std::queue<Task> tasks;

	std::mutex mtx;
	std::condition_variable cv;

	std::atomic<bool> is_running;
};