#ifndef NNGCPP_SOCKET_H
#define NNGCPP_SOCKET_H

// nng should be in the include path.
#include <functional>
#include <string>

#include "nng/core/sender.h"
#include "nng/core/receiver.h"
#include "nng/core/options.h"

#include "../nng.h"

namespace nng {

    enum protocol_type {
        protocol_none = ::NNG_PROTO_NONE,
        proto_bus = ::NNG_PROTO_BUS,
        proto_pair = ::NNG_PROTO_PAIR,
        proto_subscriber = ::NNG_PROTO_SUB,
        proto_publisher = ::NNG_PROTO_PUB,
        proto_requestor = ::NNG_PROTO_REQ,
        proto_replier = ::NNG_PROTO_REP,
        proto_pusher = ::NNG_PROTO_PUSH,
        proto_puller = ::NNG_PROTO_PULL,
        proto_surveyor = ::NNG_PROTO_SURVEYOR,
        proto_respondent = ::NNG_PROTO_RESPONDENT
    };

    protocol_type to_protocol_type(int value);

    class listener;
    class dialer;

    class NNGCPP_DECLSPEC socket : public sender, public receiver, public options {
        private:

            typedef std::size_t size_type;

            friend class listener;
            friend class dialer;

            // For use with Device Thread Callback.
            friend void thread_callback(void* arg);

            ::nng_socket sid;

        protected:

            typedef std::function<int(::nng_socket* const)> nng_ctor_func;

            socket(nng_ctor_func const& nng_ctor);

        public:

            virtual ~socket();

            void listen(std::string const& addr, int flags = 0);
            void listen(std::string const& addr, listener* const lp, int flags = 0);

            void dial(std::string const& addr, int flags = 0);
            void dial(std::string const& addr, dialer* const dp, int flags = 0);

            void close();

            void set_option(int opt, const void* val, option_size_type sz);
            void get_option(int opt, void* val, option_size_type* szp);

            // Convenience option wrappers.
            void set_option_int(int opt, int val);
            void set_option_size(int opt, option_size_type val);
            void set_option_usec(int opt, uint64_t val);

            void get_option_int(int opt, int* valp);
            void get_option_size(int opt, option_size_type* valp);
            void get_option_usec(int opt, uint64_t* valp);

            virtual int send(std::string const& str, int flags = 0);
            virtual int send(std::string const& str, send_size_type sz, int flags = 0);

            virtual int send(send_vector const& buffer, int flags = 0);
            virtual int send(send_vector const& buffer, send_size_type sz, int flags = 0);

            virtual int try_receive(std::string& str, receive_size_type& sz, int flags = 0);
            virtual std::string socket::receive_str(receive_size_type& sz, int flags = 0);

            virtual int try_receive(receive_vector& str, receive_size_type& sz, int flags = 0);
            virtual receive_vector socket::receive_buffer(receive_size_type& sz, int flags = 0);

            protocol_type get_protocol() const;
            protocol_type get_peer() const;

            // TODO: TBD: I'm not sure it makes that much quite as much sense to ask whether the socket is this that or the other protocol type...
            bool is_protocol_configured() const;
            bool is_peer_configured() const;
    };
}

#endif // NNGCPP_SOCKET_H
