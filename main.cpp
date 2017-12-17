


#include "threadpool.hpp"
#include <iostream>
using namespace std;

void f(int i) { cout << i<<' '; }
int main()
{
	ThreadPool<20> pool;
	vector<future<void>> tasks;

	for (int i = 0; i < 10000; ++i)
		tasks.emplace_back(pool.append(f, i));
	for (auto& task : tasks)
		task.get();

	std::this_thread::sleep_for(std::chrono::seconds(2));
}
