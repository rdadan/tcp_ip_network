 ///
 /// @file    Timer.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 10:58:57
 ///
 
#ifndef __WD_TIMER_H__
#define __WD_TIMER_H__

#include <functional>

namespace wd
{

class Timer
{
	using TimerCallback = std::function<void()>;
public:
	Timer(int initTime, int intervalTime, TimerCallback cb);

	void start();
	void stop();
private:
	void handleRead();

private:
	int _fd;
	int _initTime;
	int _intervalTime;
	TimerCallback _cb;
	bool _isStarted;
};

}//end of namespace wd

#endif
