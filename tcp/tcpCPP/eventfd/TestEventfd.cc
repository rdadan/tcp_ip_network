 ///
 /// @file    TestTimer.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:27:39
 ///
 
//#include "Timer.h"
#include "EventfdThread.h"
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
	wd::Eventfd efd(std::bind(&Task::process, Task()));
	//efd.start();

	wd::Thread thread(std::bind(&wd::Eventfd::start, &efd));

	thread.start();//子线程

	int cnt = 10;
	while(cnt--)
	{
		efd.wakeup();//在主线程里通知子线程去执行任务
		sleep(1);
	}

	efd.stop();
	thread.join();
	return 0;
}

#if 1
void test1(void)
{
	wd::EventfdThread efd(std::bind(&Task::process, Task()));
	
	efd.start();//开启子线程
	int cnt = 10;

	while(cnt--)
	{
		efd.wakeup();//通知子线程干活
		::sleep(1);
	}
	efd.stop();
}
#endif

int main(void)
{
	//test0();
	test1();
	return 0;
}
