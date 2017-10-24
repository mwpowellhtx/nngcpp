#ifndef NNGCPP_LISTENER_H
#define NNGCPP_LISTENER_H

#include "enums.h"
#include "endpoint.h"
#include "options.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class listener : public endpoint {
    public:

        typedef ::nng_listener nng_type;

    private:

        friend class socket;

        nng_type lid;

        options_reader_writer _options;

        void configure_options(nng_type lid);

    protected:

        virtual void on_listened();

    public:

        listener();

        listener(const socket* const sp, const std::string& addr);

        virtual ~listener();

        virtual void start(flag_type flags = flag_none);
        virtual void close() override;

        virtual bool has_one() const override;

        virtual options_reader_writer* const options();
    };
}

#endif // NNGCPP_LISTENER_H
