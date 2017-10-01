#ifndef NNGCPP_LISTENER_H
#define NNGCPP_LISTENER_H

#include "../nngcpp_integration.h"

#include "endpoint.h"
#include "options.h"

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class listener : public endpoint, public options {
    public:

        typedef ::nng_listener nng_type;

        private:

            friend class socket;

            nng_type lid;

        public:

            listener();

            listener(const socket& s, const std::string& addr);

            virtual ~listener();

            void start(int flags);
            void close();

            virtual void set_option_int(const std::string* const cnamecp, int val);
            virtual void set_option_size(const std::string* const cnamecp, option_size_type val);
            virtual void set_option_usec(const std::string* const cnamecp, option_ulonglong_type val);

            virtual void get_option_int(const std::string* const cnamecp, int* valp);
            virtual void get_option_size(const std::string* const cnamecp, option_size_type* valp);
            virtual void get_option_usec(const std::string* const cnamecp, option_ulonglong_type* valp);
    };
}

#endif // NNGCPP_LISTENER_H
