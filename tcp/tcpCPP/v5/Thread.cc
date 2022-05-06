 ///
 /// @file    Thread.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2018-03-17 09:57:57
 ///
 
#include "Thread.h"
#include <iostream>
using std::cout;
using std::endl;

namespace wd
{

namespace current_thread
{
__thread const char * threadName = "wd";
}

struct ThreadData
{
	ThreadData(const Thread::ThreadCallback & cb, const string & name)
	: _cb(cb)
	, _name(name)
	{}


	void runInThread()
	{
		current_thread::threadName = _name.empty()?"wd":_name.c_str();
		if(_cb)
			_cb();
	}

	Thread::ThreadCallback _cb;
	string _name;
};


Thread::Thread(ThreadCallback cb, const string & name)
: _pthId(0)
, _isRunning(false)
, _cb(cb)
, _name(name)
{
}

void Thread::start()
{
	ThreadData * pdata = new ThreadData(_cb, _name);
	pthread_create(&_pthId, NULL, threadFunc, pdata);
	_isRunning = true;
}

void * Thread::threadFunc(void * arg)
{
	ThreadData * pdata = static_cast<ThreadData*>(arg);
	if(pdata)
		pdata->runInThread();

	return NULL;
}

void Thread::join()
{
	pthread_join(_pthId, NULL);
	_isRunning = false;
}

Thread::~Thread()
{
	if(_isRunning)
	{
		pthread_detach(_pthId);
		_isRunning = false;
	}
	cout << "~Thread()" << endl;
}

}// end of namespace wd
