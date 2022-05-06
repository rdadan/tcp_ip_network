 ///
 /// @file    Eventfd.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-22 10:58:57
 ///
 
#ifndef __WD_EVENTFD_H__
#define __WD_EVENTFD_H__

#include <functional>

namespace wd
{

class Eventfd
{
	using EventfdCallback = std::function<void()>;
public:
	Eventfd(EventfdCallback cb);

	void start();
	void stop();
	void wakeup();
private:
	void handleRead();

private:
	int _fd;
	EventfdCallback _cb;
	bool _isStarted;
};

}//end of namespace wd

#endif
