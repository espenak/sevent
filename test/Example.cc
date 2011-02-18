#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "SocketService.h"
#include "AsioSocketService.h"
#include "AsioSocketListener.h"

using namespace socketevent;

boost::mutex global_stream_lock;

class ExceptionHandlingWorkerThread
{
    public:
        typedef void result_type;
        ExceptionHandlingWorkerThread() {}
        void operator ()(SocketService_ptr service)
        {
            while (true)
            {
                try
                {
                    service->run();
                } catch (boost::exception& e)
                {
                    boost::lock_guard<boost::mutex> lock(stream_lock);
                    std::cout << "[" << boost::this_thread::get_id()
                            << "] Exception: "
                            << boost::diagnostic_information(e) << std::endl;
                } catch (std::exception& e)
                {
                    boost::lock_guard<boost::mutex> lock(stream_lock);
                    std::cout << "[" << boost::this_thread::get_id()
                            << "] Exception: " << e.what() << std::endl;
                }
            }
        }
    private:
        boost::mutex stream_lock;
};
ExceptionHandlingWorkerThread exceptionHandlingWorkerThread;

void workerThread(SocketService_ptr service)
{
    while (true)
    {
        try
        {
            service->run();
        } catch (std::exception & ex)
        {
            global_stream_lock.lock();
            std::cout << "[" << boost::this_thread::get_id() << "] Exception: "
                    << ex.what() << std::endl;
            global_stream_lock.unlock();
        }
    }
}

int main(int argc, const char *argv[])
{
    AsioSocketService_ptr service = AsioSocketService::make();
    boost::thread_group worker_threads;
    for (int x = 0; x < 2; ++x)
    {
        worker_threads.create_thread(boost::bind(&workerThread, service));
//        worker_threads.create_thread(boost::bind(boost::ref(exceptionHandlingWorkerThread), service));
    }

    AsioSocketListener listener(service);
    listener.listen(Address::make("localhost", "9090"));

    worker_threads.join_all();

    return 0;
}
