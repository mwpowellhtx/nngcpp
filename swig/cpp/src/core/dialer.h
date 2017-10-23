#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

#include "enums.h"
#include "endpoint.h"
#include "options.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class dialer : public endpoint {
    public:

        typedef ::nng_dialer nng_type;

    private:

        friend class socket;

        nng_type did;

        options_reader_writer _options;

        void configure_options();

    protected:

        virtual void on_dialed();

    public:

        dialer();

        dialer(const socket* const sp, const std::string& addr);

        virtual ~dialer();

        virtual void start(flag_type flags = flag_none);
        virtual void close();

        virtual bool has_one() const override;

        virtual options_reader_writer* const options();
    };
}

#endif // NNGCPP_DIALER_H
