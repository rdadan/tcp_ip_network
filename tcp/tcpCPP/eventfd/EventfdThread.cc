 ///
 /// @file    EventfdThread.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:41:08
 ///
 
#include "EventfdThread.h"

using namespace wd;

EventfdThread::EventfdThread(EventfdCallback cb)
: _efd(cb)
, _thread(std::bind(&Eventfd::start, &_efd))
{}

void EventfdThread::start()
{
	_thread.start();
}

void EventfdThread::wakeup()
{
	_efd.wakeup();
}

void EventfdThread::stop()
{
	_efd.stop();
	_thread.join();
}
