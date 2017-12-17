


#include "threadpool.hpp"
#include <iostream>
using namespace std;

void f(int i) { cout << i<<' '; }
int main()
{
	ThreadPool<> pool;
	pool.run();

	auto a=pool.append(f, 2);
	auto b=pool.append(f, 3);
	auto c=pool.append(f, 4);

	a.get();
	b.get();
	c.get();




	std::this_thread::sleep_for(std::chrono::seconds(5));
}
