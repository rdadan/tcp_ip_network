 ///
 /// @file    Thread.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-17 09:51:49
 ///
 
#ifndef __WD_THREAD_H__
#define __WD_THREAD_H__

#include "Noncopyable.h"
#include <pthread.h>
#include <string>
#include <sstream>
#include <functional>
using std::string;

namespace wd
{

inline string int2str(size_t number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

inline int str2int(const string & str)
{
	std::istringstream iss(str);
	int number;
	iss >> number;
	return number;
}

namespace current_thread
{
extern __thread const char * threadName;
}

class Thread
: Noncopyable
{
public:
	using ThreadCallback = std::function<void()>;
	Thread(ThreadCallback, const string &name=string());
	~Thread();

	void start();
	void join();

private:
	static void * threadFunc(void * arg);
private:
	pthread_t _pthId;
	bool _isRunning;
	ThreadCallback _cb;//回调函数
	string _name;
};

}//end of namespace wd

#endif
