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

            virtual void start(int flags = 0);
            virtual void close();

            virtual void set_option(const std::string& name, const std::string& val, option_size_type sz);
            virtual void set_option(const std::string& name, const std::string& val);

            virtual void get_option(const std::string& name, std::string& val);
            virtual void get_option(const std::string& name, std::string& val, option_size_type& sz);

            virtual void set_option(const std::string& name, const void* valp, option_size_type sz);
            virtual void get_option(const std::string& name, void* val, option_size_type* szp);

            virtual void set_option_int(const std::string& name, int val);
            virtual void set_option_size(const std::string& name, option_size_type val);
            virtual void set_option_usec(const std::string& name, option_ulonglong_type val);

            virtual void get_option_int(const std::string& name, int* valp);
            virtual void get_option_size(const std::string& name, option_size_type* valp);
            virtual void get_option_usec(const std::string& name, option_ulonglong_type* valp);
    };
}

#endif // NNGCPP_LISTENER_H
