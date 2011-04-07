#include "Facade.h"
#include "sevent/asiosocket/AsioFacade.h"

namespace sevent
{
    namespace socket
    {
        Facade_ptr Facade::make()
        {
            return asiosocket::AsioFacade::make();
        }
    }
}
