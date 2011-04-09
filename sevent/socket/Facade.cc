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
        
        
        void Facade::removeListener(const Address& address)
        {
            _listeners.erase(address);
        }

        bool Facade::isLocalSession(Session_ptr session)
        {
            Address_ptr remoteAddr = session->getRemoteEndpointAddress();
            return _listeners.find(*remoteAddr) != _listeners.end();
        }

        void Facade::saveListener(const Address& address,
                                  Listener_ptr listener)
        {
            _listeners[address] = listener;
        }
    }
}
