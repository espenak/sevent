#include "AsioSocketFacade.h"
#include "AsioSocketSession.h"
#include "AsioSocketListener.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace sevent
{
    namespace socket
    {
        AsioSocketFacade::AsioSocketFacade()
        {
            _service = AsioSocketService::make();
            _sessionRegistry = SocketSessionRegistry::make();
            _connector = SocketConnector_ptr(new AsioSocketConnector(
                                                 _service, _sessionRegistry));
        }
        AsioSocketFacade::~AsioSocketFacade()
        {
        }

        AsioSocketFacade_ptr AsioSocketFacade::make()
        {
            return AsioSocketFacade_ptr(new AsioSocketFacade());
        }

        void AsioSocketFacade::setWorkerThreads(unsigned count,
                                                allEventsHandler_t allEventsHandler)
        {
            setWorkerThreads(count, defaultWorkerThreadHandler, allEventsHandler);
        }

        void AsioSocketFacade::setWorkerThreads(unsigned count,
                                                workerThread_t workerThreadHandler,
                                                allEventsHandler_t allEventsHandler)
        {
            _sessionRegistry->setAllEventsHandler(
                boost::bind(allEventsHandler, shared_from_this(), _1, _2));
            for (unsigned x = 0; x < count; ++x)
            {
                _worker_threads.create_thread(
                    boost::bind(workerThreadHandler, shared_from_this()));
            }
        }

        SocketListener_ptr AsioSocketFacade::listen(Address_ptr address)
        {
            SocketListener_ptr listener = boost::make_shared<AsioSocketListener>(_service, _sessionRegistry);
            listener->listen(address);
            return listener;
        }

        SocketSession_ptr AsioSocketFacade::connect(Address_ptr address)
        {
            return _connector->connect(address);
        }

        SocketService_ptr AsioSocketFacade::service()
        {
            return _service;
        }

        void AsioSocketFacade::joinAllWorkerThreads()
        {
            _worker_threads.join_all();
        }


        void AsioSocketFacade::defaultWorkerThreadHandler(SocketFacade_ptr facade)
        {
            try
            {
                facade->service()->run();
            }
            catch (boost::exception& e)
            {
                std::cerr << "[" << boost::this_thread::get_id()
                          << "] Exception: " << boost::diagnostic_information(e)
                          << std::endl;
            }
            catch (std::exception& e)
            {
                std::cerr << "[" << boost::this_thread::get_id()
                          << "] Exception: " << e.what() << std::endl;
            }
        }
    }
}
