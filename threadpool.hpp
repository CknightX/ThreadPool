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
	template<class Func,class... Args>
	auto append(Func&& f,Args&&... args)->std::future<decltype(f(args...))>
	{
		if (!is_running.load())
			throw std::runtime_error("ThreadPool is not running now.");
		using RetType = decltype(f(args...));

		auto task = std::make_shared<std::packaged_task<RetType()>>(
			std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
		);

		std::future<RetType> res = task->get_future();

		{
			std::unique_lock<std::mutex> lck(mtx);
			tasks.emplace([task]() {(*task)(); });
		}

		cv.notify_one();
		return res;

	}
	~ThreadPool()
	{
		stop();
		// wake up all
		cv.notify_all();

		// join all threads
		for (auto& t : pool)
			t.join();
	}
private:
	// get one task.
	Task get_task() 
	{
		std::unique_lock<std::mutex> lck(mtx);
	//	cv.wait(lck, [this] {return !tasks.empty(); });
		while (tasks.empty() && is_running)
			cv.wait(lck);

		// ~ThreadPool()
		if (tasks.empty() && !is_running)
			return nullptr;

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
				return;
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