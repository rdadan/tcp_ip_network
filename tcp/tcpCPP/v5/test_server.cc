
#include "TcpServer.h"
#include "Threadpool.h"
#include "Thread.h"
#include <iostream>
using std::cout;
using std::endl;

class Task
{
public:
	Task(const string &query, const wd::TcpConnectionPtr &conn)
		: _queury(query), _conn(conn)
	{
	}

	void process() //工作在计算线程  pthread_self()
	{
		cout << "> thread " << wd::current_thread::threadName << ": task is processing" << endl;
		//_conn->send(_queury);//如果直接去使用send，在计算线程里面完成了数据发送,不可取
		_conn->sendInLoop(_queury);
		// 1. 先去cache之中进行查找（使用线程局部存储 __thread拿到相应cache的下标idx）
		int idx = wd::str2int(wd::current_thread::threadName);
		cout << "thread idx = " << idx << endl;

		// 2. 当cache之中没有结果时候，再去执行查询模块
		//...
	}

private:
	string _queury;
	wd::TcpConnectionPtr _conn;
};

wd::Threadpool *g_threadpool = NULL;

void onConnection(const wd::TcpConnectionPtr &conn)
{
	cout << conn->toString() << endl;
	conn->send("hello, welcome to Chat Server.\r\n");
}

//运行在IO线程
void onMessage(const wd::TcpConnectionPtr &conn)
{
	std::string s(conn->receive());

	Task task(s, conn);
	g_threadpool->addTask(std::bind(&Task::process, task));
	cout << "> add task to threadpool" << endl;
}

void onClose(const wd::TcpConnectionPtr &conn)
{
	printf("%s close\n", conn->toString().c_str());
}

#if 0
//使用TcpServer和Threadpool的模板
class EchoServer;
{
public:
	onMessage(const wd::TcpConnectionPtr & conn);
	onConnection(const wd::TcpConnectionPtr & conn);
	onClose(const wd::TcpConnectionPtr & conn);
private:
	Threadpool _threadpool;
	TcpServer _server;
}
#endif

int main(int argc, char const *argv[])
{
	wd::Threadpool threadpool(4, 10);
	g_threadpool = &threadpool;
	threadpool.start();

	wd::TcpServer tcpserver(8888);
	tcpserver.setConnectionCallback(&onConnection);
	tcpserver.setMessageCallback(&onMessage);
	tcpserver.setCloseCallback(&onClose);

	tcpserver.start();

	return 0;
}
