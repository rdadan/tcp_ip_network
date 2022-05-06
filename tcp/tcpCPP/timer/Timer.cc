 ///
 /// @file    Timer.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:02:15
 ///
 
#include "Timer.h"

#include <unistd.h>
#include <sys/timerfd.h>
#include <poll.h>

#include <iostream>
using std::cout;
using std::endl;
 
namespace wd
{
inline int createTimerFd()
{
	int fd = ::timerfd_create(CLOCK_REALTIME, 0); 
	if(fd == -1)
	{
		perror("timerfd_create");	
	}
	return fd;
} 


inline
void setTimerFd(int fd, int initTime, int intervalTime)
{
	struct itimerspec value;
	value.it_value.tv_sec = initTime;
	value.it_value.tv_nsec = 0;
	value.it_interval.tv_sec = intervalTime;
	value.it_interval.tv_nsec = 0;

	int ret = ::timerfd_settime(fd, 0, &value, NULL);
	if(ret == -1)
	{
		perror("timerfd_settime");
	}
}


Timer::Timer(int initTime, int intervalTime, TimerCallback cb)
: _fd(createTimerFd())
, _initTime(initTime)
, _intervalTime(intervalTime)
, _cb(cb)
{

}

void Timer::start()
{
	struct pollfd pfd;
	pfd.fd = _fd;
	pfd.events = POLLIN;

	setTimerFd(_fd, _initTime, _intervalTime);//开启定时器

	_isStarted = true;
	while(_isStarted)
	{
		int nready = ::poll(&pfd, 1, 5000);
		if(nready == -1 && errno == EINTR)
			continue;
		else if(nready == -1)
		{
			perror("poll error");
			return;
		}
		else if(nready == 0)
			cout << "> poll timeout." << endl;
		else
		{
			if(pfd.revents & POLLIN)
			{
				handleRead();
				if(_cb)
					_cb();
			}
		}
	}
}

void Timer::stop()
{
	if(_isStarted)
	{
		_isStarted = false;
		setTimerFd(_fd, 0, 0);
	}
}

 
void Timer::handleRead()
{
	uint64_t howmany;
	int ret = ::read(_fd, &howmany, sizeof(howmany));
	if(ret != sizeof(howmany))
	{
		perror("read");
	}
}
}//end of namespace wd
