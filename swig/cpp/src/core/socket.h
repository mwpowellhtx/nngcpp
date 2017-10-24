#ifndef NNGCPP_SOCKET_H
#define NNGCPP_SOCKET_H

#include "types.h"

#include "enums.h"
#include "sender.h"
#include "receiver.h"
#include "messenger.h"
#include "options.h"

#include "having_one.hpp"

// nng should be in the include path.
#include <functional>
#include <string>

namespace nng {

    protocol_type to_protocol_type(int value);
    int to_int(const protocol_type value);

    class listener;
    class dialer;
    struct device_path;

    class socket : public having_one, public sender, public receiver, public messenger {
    public:

        typedef ::nng_socket nng_type;

    private:

        friend class listener;
        friend class dialer;

        // For use with Device Thread Callback.
        friend void install_device_sockets_callback(const device_path* const);

        nng_type sid;

        options_reader_writer _options;

        void configure_options(nng_type sid);

    protected:

        typedef std::function<int(nng_type* const)> nng_ctor_func;

        socket(const nng_ctor_func& nng_ctor);

    public:

        virtual ~socket();

        // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
        void listen(const std::string& addr, flag_type flags = flag_none);
        void listen(const std::string& addr, listener* const lp, flag_type flags = flag_none);

        void dial(const std::string& addr, flag_type flags = flag_none);
        void dial(const std::string& addr, dialer* const dp, flag_type flags = flag_none);

        void close();
        void shutdown();

        virtual bool has_one() const override;

        virtual options_reader_writer* const options();

        virtual void send(binary_message* const bmp, flag_type flags = flag_none) override;

        virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override;
        virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override;

        virtual std::unique_ptr<binary_message> receive(flag_type flags = flag_none) override;
        virtual bool try_receive(binary_message* const bmp, flag_type flags = flag_none) override;

        virtual buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) override;
        virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;

        protocol_type get_protocol() const;
        protocol_type get_peer() const;
    };
}

#endif // NNGCPP_SOCKET_H
