#ifndef NNGCPP_LISTENER_H
#define NNGCPP_LISTENER_H

#include "enums.h"
#include "endpoint.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

    class _Listener : public _EndPoint {
    public:

        typedef ::nng_listener nng_type;

    private:

        friend class _Socket;

        nng_type lid;

        void configure_options(nng_type lid);

    protected:

        virtual void on_listened();

    public:

        _Listener();

        _Listener(const _Socket& s, const std::string& addr);

        virtual ~_Listener();

        virtual void Start() override;

        virtual void Start(SocketFlag flags) override;

        virtual void Close() override;

        virtual bool HasOne() const override;

        virtual _OptionReaderWriter* const GetOptions() override;
    };

    typedef _Listener listener;
}

#endif // NNGCPP_LISTENER_H
