 ///
 /// @file    TestTimer.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:27:39
 ///
 
//#include "Timer.h"
//#include "Thread.h"
#include "TimerThread.h"
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
using std::cout;
using std::endl;

struct Task
{
	void process()
	{
		::srand(::time(NULL));
		int number = ::rand() % 100;
		cout << "> number = " << number << endl;
	}
};
 
int test0(void)
{
	wd::Timer timer(3, 6, std::bind(&Task::process, Task()));

	wd::Thread subThread(std::bind(&wd::Timer::start, &timer));
	subThread.start();
	::sleep(20);
	timer.stop();
	subThread.join();
	return 0;
}

void test1(void)
{
	wd::TimerThread timer(3, 5, std::bind(&Task::process, Task()));
	timer.start();

	::sleep(20);

	timer.stop();
}

int main(void)
{
	test1();
	return 0;
}
