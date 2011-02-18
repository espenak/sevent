#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include  <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "SocketService.h"
#include "AsioSocketService.h"
#include "AsioSocketListener.h"
#include "AsioSocketConnector.h"
#include "WorkerThread.h"

using namespace socketevent;
boost::condition_variable listenerReady;
boost::mutex listenerReadyMutex;

void stopEvent(AsioSocketService_ptr service) {
    service->stop();
}


int main(int argc, const char *argv[])
{
    AsioSocketService_ptr service = AsioSocketService::make();
    boost::thread_group worker_threads;
    for (int x = 0; x < 2; ++x)
    {
        worker_threads.create_thread(boost::bind(boost::ref(exceptionHandlingWorkerThread), service));
    }

    SocketSessionRegistry_ptr socketSessionRegistry = SocketSessionRegistry::make();
    AsioSocketListener listener(service, socketSessionRegistry);
    listener.listen(Address::make("localhost", "9090"));
    std::cout << "Listening..." << std::endl;

    AsioSocketConnector client(service);
    SocketSession_ptr session = client.connect(Address::make("localhost", "9090"));
    char data[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    session->sendEvent(10, Buffer(data, 6));
    worker_threads.join_all();

    return 0;
}
