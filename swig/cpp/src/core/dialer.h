#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

#include "enums.h"
#include "endpoint.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class dialer : public _EndPoint {
    public:

        typedef ::nng_dialer nng_type;

    private:

        friend class socket;

        nng_type did;

        void configure_options(nng_type did);

    protected:

        virtual void on_dialed();

    public:

        dialer();

        dialer(const socket* const sp, const std::string& addr);

        virtual ~dialer();

        virtual void start(flag_type flags = flag_none);

        _OptionReaderWriter* const GetOptions() override;

        virtual void Close() override;

        virtual bool HasOne() const override;
    };
}

#endif // NNGCPP_DIALER_H
