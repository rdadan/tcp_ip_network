#include "producer.h"
#include "consumer.h"
#include "taskQueue.h"
#include <memory>
using std::unique_ptr;

using namespace dd;
int main(void)
{
    TaskQueue queue(10);

    unique_ptr<Thread> pProducer(new Producer(queue));
    unique_ptr<Thread> pConsumer(new Consumer(queue));

    pProducer->startThread();
    pConsumer->startThread();

    pProducer->joinThread();
    pConsumer->joinThread();

    return 0;
}