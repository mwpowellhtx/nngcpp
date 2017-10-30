#ifndef NNGCPP_SOCKET_H
#define NNGCPP_SOCKET_H

#include "types.h"

#include "enums.h"
#include "ISender.h"
#include "IReceiver.h"
#include "../options/options.h"

#include "IHaveOne.hpp"
#include "ICanClose.hpp"

// nng should be in the include path.
#include <functional>
#include <string>

namespace nng {

    class listener;
    class dialer;
    struct device_path;

    class socket
        : public IHaveOne
        , public ICanClose
        , public ISender
        , public IReceiver
        , public IHaveOptions<_OptionReaderWriter> {
    public:

        typedef ::nng_socket nng_type;

    private:

        typedef std::function<nng::uint16_t()> get_protocol_func;

        static protocol_type __get_protocol(const get_protocol_func& op);

        friend class listener;
        friend class dialer;

        // For use with Device Thread Callback.
        friend void install_device_sockets_callback(const device_path* const);

        nng_type sid;

        void configure_options(nng_type sid);

    protected:

        typedef std::function<int(nng_type* const)> nng_ctor_func;

        socket(const nng_ctor_func& nng_ctor);

    public:

        virtual ~socket();

        virtual _OptionReaderWriter* const GetOptions() override;

        // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
        void listen(const std::string& addr, flag_type flags = flag_none);
        void listen(const std::string& addr, listener* const lp, flag_type flags = flag_none);

        void dial(const std::string& addr, flag_type flags = flag_none);
        void dial(const std::string& addr, dialer* const dp, flag_type flags = flag_none);

        void Close();
        void shutdown();

        virtual bool HasOne() const override;

        virtual void send(binary_message* const bmp, flag_type flags = flag_none) override;

        virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override;
        virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override;

        virtual void send_async(const basic_async_service* const svcp) override;

        virtual std::unique_ptr<binary_message> receive(flag_type flags = flag_none) override;
        virtual bool try_receive(binary_message* const bmp, flag_type flags = flag_none) override;

        virtual buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) override;
        virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;

        virtual void receive_async(basic_async_service* const svcp) override;

        protocol_type get_protocol() const;
        protocol_type get_peer() const;
    };
}

#endif // NNGCPP_SOCKET_H
