 ///
 /// @file    TimerThread.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:41:08
 ///
 
#include "TimerThread.h"

using namespace wd;

TimerThread::TimerThread(int initTime, int intervalTime, TimerCallback cb)
: _timer(initTime, intervalTime, cb)
, _thread(std::bind(&Timer::start, &_timer))
{}

void TimerThread::start()
{
	_thread.start();
}

void TimerThread::stop()
{
	_timer.stop();
	_thread.join();
}
