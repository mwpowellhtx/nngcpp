#ifndef NNGCPP_ADDRESS_H
#define NNGCPP_ADDRESS_H

#include "../nngcpp_integration.h"

#include <nng/nng.h>

#include <cstddef>

namespace nng {

    enum sockaddr_family_type {
        af_unspec = ::NNG_AF_UNSPEC,
        af_inproc = ::NNG_AF_INPROC,
        af_ipc = ::NNG_AF_IPC,
        af_inet = ::NNG_AF_INET,
        af_inet6 = ::NNG_AF_INET6,
        af_zt = ::NNG_AF_ZT,  // ZeroTier
    };

    class address {
    public:

        typedef ::nng_sockaddr sockaddr_type;
        typedef  std::size_t size_type;

    private:

        sockaddr_type _sa;

    public:

        address();

        virtual ~address();

        bool has_one() const;

        size_type get_size() const;

        sockaddr_type* get();

        sockaddr_family_type get_s_family() const;

        uint32_t get_addr() const;

        uint16_t get_port() const;
    };
}

#endif // NNGCPP_ADDRESS_H
