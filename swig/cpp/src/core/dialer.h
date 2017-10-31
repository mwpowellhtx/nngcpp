#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

#include "enums.h"
#include "endpoint.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

    class _Dialer : public _EndPoint {
    public:

        typedef ::nng_dialer nng_type;

    private:

        friend class _Socket;

        nng_type did;

        void configure_options(nng_type did);

    protected:

        virtual void on_dialed();

    public:

        _Dialer();

        _Dialer(const _Socket& s, const std::string& addr);

        virtual ~_Dialer();

        virtual void Start() override;

        virtual void Start(SocketFlag flags) override;

        virtual void Close() override;

        virtual bool HasOne() const override;

        virtual _OptionReaderWriter* const GetOptions() override;
    };

    typedef _Dialer dialer;
}

#endif // NNGCPP_DIALER_H
