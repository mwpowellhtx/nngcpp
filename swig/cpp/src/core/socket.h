#ifndef NNGCPP_SOCKET_H
#define NNGCPP_SOCKET_H

#include "../nngcpp_integration.h"

// nng should be in the include path.
#include <functional>
#include <string>

#include "sender.h"
#include "receiver.h"
#include "options.h"

namespace nng {

    enum protocol_type {
        protocol_none = ::NNG_PROTO_NONE,

        proto_bus_v0 = ::NNG_PROTO_BUS_V0,
        proto_pair_v0 = ::NNG_PROTO_PAIR_V0,
        proto_pair_v1 = ::NNG_PROTO_PAIR_V1,
        proto_subscriber_v0 = ::NNG_PROTO_SUB_V0,
        proto_publisher_v0 = ::NNG_PROTO_PUB_V0,
        proto_requestor_v0 = ::NNG_PROTO_REQ_V0,
        proto_replier_v0 = ::NNG_PROTO_REP_V0,
        proto_pusher_v0 = ::NNG_PROTO_PUSH_V0,
        proto_puller_v0 = ::NNG_PROTO_PULL_V0,
        proto_surveyor_v0 = ::NNG_PROTO_SURVEYOR_V0,
        proto_respondent_v0 = ::NNG_PROTO_RESPONDENT_V0,
        proto_star_v0 = ::NNG_PROTO_STAR_V0,

        proto_bus = ::NNG_PROTO_BUS,
        proto_pair = ::NNG_PROTO_PAIR,
        proto_subscriber = ::NNG_PROTO_SUB,
        proto_publisher = ::NNG_PROTO_PUB,
        proto_requestor = ::NNG_PROTO_REQ,
        proto_replier = ::NNG_PROTO_REP,
        proto_pusher = ::NNG_PROTO_PUSH,
        proto_puller = ::NNG_PROTO_PULL,
        proto_surveyor = ::NNG_PROTO_SURVEYOR,
        proto_respondent = ::NNG_PROTO_RESPONDENT,
        // TODO: TBD: NNG_PROTO_STAR undefined as of now; expecting NNG_PROTO_STAR moving forward? with protocol to boot?
        proto_star = ::NNG_PROTO_STAR_V0
    };

    protocol_type to_protocol_type(int value);

    class listener;
    class dialer;
    struct device_path;

    class socket : public sender, public receiver, public options {
        private:

            typedef std::size_t size_type;

            friend class listener;
            friend class dialer;

            // For use with Device Thread Callback.
            friend void install_device_sockets_callback(const device_path* const);

            ::nng_socket sid;

        protected:

            typedef std::function<int(::nng_socket* const)> nng_ctor_func;

            socket(const nng_ctor_func& nng_ctor);

        public:

            virtual ~socket();

            // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
            void listen(const std::string& addr, int flags = 0);
            void listen(const std::string& addr, listener* const lp, int flags = 0);

            void dial(const std::string& addr, int flags = 0);
            void dial(const std::string& addr, dialer* const dp, int flags = 0);

            void close();

            // Convenience option wrappers.
            void set_option_int(const std::string* const cnamecp, int val);
            void set_option_size(const std::string* const cnamecp, option_size_type val);
            void set_option_usec(const std::string* const cnamecp, uint64_t val);

            void get_option_int(const std::string* const cnamecp, int* valp);
            void get_option_size(const std::string* const cnamecp, option_size_type* valp);
            void get_option_usec(const std::string* const cnamecp, uint64_t* valp);

            virtual int send(const std::string& str, int flags = 0);
            virtual int send(const std::string& str, send_size_type sz, int flags = 0);

            virtual int send(const send_vector& buffer, int flags = 0);
            virtual int send(const send_vector& buffer, send_size_type sz, int flags = 0);

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
