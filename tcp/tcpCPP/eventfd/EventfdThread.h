 ///
 /// @file    EventfdThread.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:38:50
 ///
 
#ifndef __WD_EVENTFDTHREAD_H__
#define __WD_EVENTFDTHREAD_H__

#include "Eventfd.h"
#include "Thread.h"
#include <functional>

namespace wd
{

class EventfdThread
{
	using EventfdCallback = std::function<void()>;
public:
	EventfdThread(EventfdCallback);

	void start();
	void stop();
	void wakeup();
private:
	Eventfd _efd;
	Thread _thread;
};

}//end of namespace wd

#endif
