#ifndef NNGCPP_LISTENER_H
#define NNGCPP_LISTENER_H

#include "enums.h"
#include "endpoint.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class listener : public _EndPoint {
    public:

        typedef ::nng_listener nng_type;

    private:

        friend class socket;

        nng_type lid;

        void configure_options(nng_type lid);

    protected:

        virtual void on_listened();

    public:

        listener();

        listener(const socket* const sp, const std::string& addr);

        virtual ~listener();

        virtual _OptionReaderWriter* const GetOptions() override;

        virtual void start(flag_type flags = flag_none);

        virtual void Close() override;

        virtual bool HasOne() const override;
    };
}

#endif // NNGCPP_LISTENER_H
