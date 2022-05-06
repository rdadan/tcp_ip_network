 ///
 /// @file    TimerThread.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:38:50
 ///
 
#ifndef __WD_TIMERTHREAD_H__
#define __WD_TIMERTHREAD_H__

#include "Timer.h"
#include "Thread.h"
#include <functional>

namespace wd
{

class TimerThread
{
	using TimerCallback = std::function<void()>;
public:
	TimerThread(int,int, TimerCallback);

	void start();
	void stop();
private:
	Timer _timer;
	Thread _thread;
};

}//end of namespace wd

#endif
