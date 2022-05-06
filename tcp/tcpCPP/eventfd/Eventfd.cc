 ///
 /// @file    Eventfd.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 11:02:15
 ///
 
#include "Eventfd.h"

#include <unistd.h>
#include <sys/eventfd.h>
#include <poll.h>

#include <iostream>
using std::cout;
using std::endl;
 
namespace wd
{
inline int createEventFd()
{
	int fd = ::eventfd(0, 0); 
	if(fd == -1)
	{
		perror("eventfd");	
	}
	return fd;
} 



Eventfd::Eventfd(EventfdCallback cb)
: _fd(createEventFd())
, _cb(cb)
{
}

void Eventfd::start()
{
	struct pollfd pfd;
	pfd.fd = _fd;
	pfd.events = POLLIN;

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

void Eventfd::stop()
{
	if(_isStarted)
	{
		_isStarted = false;
	}
}

//该方法运行在A线程中
void Eventfd::wakeup()
{
	uint64_t one = 1;
	int ret = ::write(_fd, &one, sizeof(one));
	if(ret != sizeof(one))
	{
		perror("write error");
	}
}
 
//该方法运行在B线程中
void Eventfd::handleRead()
{
	uint64_t howmany;
	int ret = ::read(_fd, &howmany, sizeof(howmany));
	if(ret != sizeof(howmany))
	{
		perror("read");
	}
}
}//end of namespace wd
