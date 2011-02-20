#pragma once
#include "Address.h"
#include "AsioService.h"

namespace sevent {
    namespace asiosocket {
        socket::Address_ptr resolveToAddress(AsioService_ptr socketservice,
                const std::string& host, unsigned short port);
    } // namespace asiosocket
} // namespace sevent
