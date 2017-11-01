#ifndef NNGCPP_SOCKET_H
#define NNGCPP_SOCKET_H

#include "types.h"

#include "enums.h"
#include "ISender.h"
#include "IReceiver.h"
#include "IProtocol.h"
#include "../options/options.h"

#include "IHaveOne.hpp"
#include "ICanClose.hpp"
#include "ICanListen.hpp"
#include "ICanDial.hpp"

#include "exceptions.hpp"

#define THROW_SOCKET_INV_OP(s, op) throw nng::exceptions::invalid_operation(#s " cannot " #op)

// nng should be in the include path.
#include <functional>
#include <string>

namespace nng {

    class _Listener;
    class _Dialer;
    struct device_path;

    class _Socket
        : public IHaveOne
        , public IProtocol
        , public ICanClose
        , public ICanListen
        , public ICanDial
        , public ISender
        , public IReceiver
        , public IHaveOptions<_OptionReaderWriter> {
    public:

        typedef ::nng_socket nng_type;

    private:

        friend class _Listener;
        friend class _Dialer;

        // For use with Device Thread Callback.
        friend void install_device_sockets_callback(const device_path* const);

        nng_type sid;

        friend nng_type get_sid(const _Socket&);

        void configure_options(nng_type sid);

    protected:

        typedef std::function<int(nng_type* const)> nng_ctor_func;

        _Socket(const nng_ctor_func& nng_ctor);

    public:

        virtual ~_Socket();

        virtual _OptionReaderWriter* const GetOptions() override;

        // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
        virtual void Listen(const std::string& addr, flag_type flags = flag_none) override;
        virtual void Listen(const std::string& addr, _Listener* const lp, flag_type flags = flag_none) override;

        virtual void Dial(const std::string& addr, flag_type flags = flag_none) override;
        virtual void Dial(const std::string& addr, _Dialer* const dp, flag_type flags = flag_none) override;

        virtual void Close() override;

        virtual bool HasOne() const override;

        virtual void Send(binary_message& m, flag_type flags = flag_none) override;

        virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) override;
        virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) override;

        virtual void SendAsync(const basic_async_service* const svcp) override;

        virtual std::unique_ptr<binary_message> Receive(flag_type flags = flag_none) override;
        virtual bool TryReceive(binary_message* const bmp, flag_type flags = flag_none) override;

        virtual buffer_vector_type Receive(size_type& sz, flag_type flags = flag_none) override;
        virtual bool TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;

        virtual void ReceiveAsync(basic_async_service* const svcp) override;
    };
}

#endif // NNGCPP_SOCKET_H
