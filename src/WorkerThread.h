#pragma once

namespace socketevent
{

class ExceptionHandlingWorkerThread: boost::noncopyable
{
    public:
        typedef void result_type;
        void operator ()(SocketService_ptr service)
        {
            try
            {
                service->run();
            } catch (boost::exception& e)
            {
                boost::lock_guard<boost::mutex> lock(stream_lock);
                std::cout << "[" << boost::this_thread::get_id()
                        << "] Exception: " << boost::diagnostic_information(e)
                        << std::endl;
            } catch (std::exception& e)
            {
                boost::lock_guard<boost::mutex> lock(stream_lock);
                std::cout << "[" << boost::this_thread::get_id()
                        << "] Exception: " << e.what() << std::endl;
            }
        }
    private:
        boost::mutex stream_lock;
};
ExceptionHandlingWorkerThread exceptionHandlingWorkerThread;

} // namespace socketevent
