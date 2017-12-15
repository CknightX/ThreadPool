


#include "threadpool.hpp"
#include <iostream>
using namespace std;

void f(int i) { cout << i<<' '; }
int main()
{
	ThreadPool<> pool;
	int i = 10000;
	pool.run();
	while (i-- > 0)
	{
		pool.append(std::bind(f,i));
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
}
