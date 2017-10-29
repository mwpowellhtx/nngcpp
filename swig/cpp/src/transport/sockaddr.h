#ifndef NNGCPP_TRANSPORT_SOCKADDR_H
#define NNGCPP_TRANSPORT_SOCKADDR_H

#include "../core/types.h"

namespace nng {

    /* Some address details. This is in some ways like a traditional sockets sockaddr,
    but we have our own to cope with our unique families, etc. The details of this
    structure are directly exposed to applications. These structures can be obtained
    via property lookups, etc. */
    struct sockaddr_path_t {
        nng::uint16_t sa_family;
        char sa_path[NNG_MAXADDRLEN];
    };
    typedef struct sockaddr_path_t sockaddr_path_t;
    typedef struct sockaddr_path_t sockaddr_ipc_t;
    typedef struct sockaddr_path_t sockaddr_inproc_t;

    // TODO: TBD: I am prepared to support this path for accessing the IPv6 address regardless what Garrett does in the NNG code...
    // TODO: TBD have a look at: https://stackoverflow.com/questions/22622013/wrap-existing-memory-with-const-stdvector#answer-22622361
    // for potential approach to exposing the value of the array in a const-vector to a reference wrapper...
    // https://stackoverflow.com/questions/6623387/fix-lock-size-of-stdvector
    // https://stackoverflow.com/questions/2434196/how-to-initialize-stdvector-from-c-style-array
    struct sockaddr_in6_t {
        nng::uint16_t sa_family;
        nng::uint16_t sa_port;
        union {
            nng::uint8_t sa_addr[16];
            nng::uint16_t sa_addr16[8];
            nng::uint32_t sa_addr32[4];
        };
    };
    typedef struct sockaddr_in6_t sockaddr_in6_t;
    typedef struct sockaddr_in6_t sockaddr_udp6_t;
    typedef struct sockaddr_in6_t sockaddr_tcp6_t;

    struct sockaddr_unspec_t {
        nng::uint16_t sa_family;
    };

    struct sockaddr_in_t {
        nng::uint16_t sa_family;
        nng::uint16_t sa_port;
        nng::uint32_t sa_addr;
    };

    struct sockaddr_zt_t {
        nng::uint16_t sa_family;
        nng::uint64_t sa_nwid;
        nng::uint64_t sa_nodeid;
        nng::uint32_t sa_port;
    };

    typedef struct sockaddr_in_t sockaddr_in_t;
    typedef struct sockaddr_in_t sockaddr_udp_t;
    typedef struct sockaddr_in_t sockaddr_tcp_t;
    typedef struct sockaddr_zt_t sockaddr_zt_t;

    typedef struct sockaddr_t {
        union {
            nng::uint16_t s_family;
            sockaddr_path_t s_path;
            sockaddr_inproc_t s_inproc;
            sockaddr_in6_t s_in6;
            sockaddr_in_t s_in;
            sockaddr_zt_t s_zt;
        } s_un;
    } sockaddr_t;
}

#endif // NNGCPP_TRANSPORT_SOCKADDR_H
