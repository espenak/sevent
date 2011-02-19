#include "SocketFacade.h"
#include "AsioSocketFacade.h"

namespace sevent
{
    namespace socket
    {
        SocketFacade_ptr SocketFacade::make()
        {
            return AsioSocketFacade::make();
        }
    }
}
