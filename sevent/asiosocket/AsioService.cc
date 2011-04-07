#include "AsioService.h"

namespace sevent
{
    namespace asiosocket
    {

        AsioService::AsioService() :
            _work(_io_service)
        {
        }

        AsioService::~AsioService()
        {
        }

        AsioService_ptr AsioService::make()
        {
            return AsioService_ptr(new AsioService());
        }

        void AsioService::run()
        {
            _io_service.run();
        }

        void AsioService::stop()
        {
            _io_service.stop();
        }

    } // namespace asiosocket
} // namespace sevent
