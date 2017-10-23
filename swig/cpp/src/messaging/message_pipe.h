#ifndef NNGCPP_MESSAGE_PIPE_H
#define NNGCPP_MESSAGE_PIPE_H

#define NNG_ONLY
#include <nngcpp.h>

#include "message_base.h"
#include "../core/options.h"

#include <algorithm>

/* Due to some namespace weirdness during the include graph forward declarations, this could
not live in another namespace for whatever reason. After careful consideration, decided that
Message Pipe is closer in stature to Dialer, Listener, and even Socket, even though it bridges
the API into Messaging. Therefore, decided that it should live in the nng::* namespace. */

namespace nng {

#ifndef NNGCPP_ADDRESS_H
    class address;
#endif //NNGCPP_ADDRESS_H

#ifndef NNGCPP_BINARY_MESSAGE_H
    template<class Body_, class Header_> class basic_binary_message;
#endif // NNGCPP_BINARY_MESSAGE_H

    // TODO: so, equal_to is available, but not_equal_to is not?
    class message_pipe : public std::equal_to<message_pipe> {
    public:

        typedef ::nng_pipe nng_type;

    protected:

        friend message_pipe::nng_type get_id(const message_pipe& mp);

        nng_type pid;

    private:

        options_reader _options;

        void reconfigure_options();

    public:

        message_pipe(messaging::message_base* const mbp);

        virtual ~message_pipe();

        virtual void close();

        virtual bool has_one() const;

        virtual options_reader* const options();

        bool operator==(const message_pipe& rhs);
        // TODO: TBD: so, apparently there is no std::not_equal_to available from the Microsoft implementation, however, we should still be able to override the operator
        bool operator!=(const message_pipe& rhs);
    };

    message_pipe::nng_type get_id(const message_pipe& mp);
}

#endif // NNGCPP_MESSAGE_PIPE_H
