


#include "threadpool.hpp"
#include <iostream>
using namespace std;

// sum: 1+2+...+desti
int part_sum(int thread_no,int thread_num,int desti)
{
	int sum = 0;
	int num = desti / thread_num;
	int i;
	for (i = thread_no * num + 1; i <= thread_no * num + num; ++i)
		sum += i;
	if (desti%thread_num != 0 && thread_no == thread_num - 1)
		for (; i <= desti; ++i)
			sum += i;
	return sum;
}
constexpr int thread_num = 111;
constexpr int desti = 1000;
int main()
{
	ThreadPool<thread_num> pool;
	vector<future<int>> tasks;
	for (int i = 0; i < thread_num; ++i)
		tasks.emplace_back(pool.append(part_sum, i, thread_num, desti));
	
	int sum = 0;
	for (auto& task : tasks)
		sum += task.get();

	cout << "number of threads: " << thread_num<<endl;
	cout << "1+2+...+" << desti << "=" << sum << endl;

	std::this_thread::sleep_for(std::chrono::seconds(3));
}
