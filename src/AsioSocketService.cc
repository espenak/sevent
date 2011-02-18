#include "AsioSocketService.h"

namespace socketevent
{

AsioSocketService::AsioSocketService() :
    _work(_io_service)
{
}

AsioSocketService::~AsioSocketService()
{}

void AsioSocketService::run()
{
    _io_service.run();
}

}
