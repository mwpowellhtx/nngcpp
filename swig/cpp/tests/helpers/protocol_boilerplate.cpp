#include "protocol_boilerplate.hpp"

#include "../../../../repos/nng/src/core/defs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    extern int nni_sock_find(nni_sock**, nng::uint32_t);
    extern nng::uint16_t nni_sock_proto(nni_sock*);
    extern nng::uint16_t nni_sock_peer(nni_sock*);
    extern void nni_sock_rele(nni_sock*);

#ifdef __cplusplus
}
#endif // __cplusplus

namespace nng {

    ::nng_socket get_sid(const _Socket& s) {
        return s.sid;
    }

    namespace protocol {

        /* Shimming these in in lieu of at least an ::nni_protocol and ::nni_peer refactoring
        from the NNG side. Which requires some externs defined in order for us to "see" the
        required NNI functionality. */

        nng::uint16_t __get_protocol(::nng_socket sid) {
            int rv;
            nng::uint16_t pnum;
            ::nni_sock *sock;

            if ((rv = ::nni_sock_find(&sock, sid)) != 0) {
                return (rv);
            }
            pnum = ::nni_sock_proto(sock);
            ::nni_sock_rele(sock);
            return (pnum);
        }

        nng::uint16_t __get_peer(::nng_socket sid) {
            int rv;
            nng::uint16_t pnum;
            ::nni_sock *sock;

            if ((rv = ::nni_sock_find(&sock, sid)) != 0) {
                return (rv);
            }
            pnum = ::nni_sock_peer(sock);
            ::nni_sock_rele(sock);
            return (pnum);
        }
    }
}
