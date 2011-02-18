#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "SocketService.h"
#include "AsioSocketService.h"
#include "AsioSocketListener.h"
#include "WorkerThread.h"

using namespace socketevent;


int main(int argc, const char *argv[])
{
    AsioSocketService_ptr service = AsioSocketService::make();
    boost::thread_group worker_threads;
    for (int x = 0; x < 2; ++x)
    {
        worker_threads.create_thread(boost::bind(boost::ref(exceptionHandlingWorkerThread), service));
    }

    AsioSocketListener listener(service);
    std::cout << "Listening..." << std::endl;
    listener.listen(Address::make("localhost", "9090"));

    worker_threads.join_all();

    return 0;
}
