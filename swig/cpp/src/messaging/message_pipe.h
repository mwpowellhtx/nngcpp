#ifndef NNGCPP_MESSAGE_PIPE_H
#define NNGCPP_MESSAGE_PIPE_H

#define NNG_ONLY
#include <nngcpp.h>

#include "../core/options.h"

#include <algorithm>

namespace nng {

#ifndef NNGCPP_ADDRESS_H
    class address;
#endif //NNGCPP_ADDRESS_H

    namespace messaging {

#ifndef NNGCPP_MESSAGE_BASE_H
        class message_base;
#endif // NNGCPP_MESSAGE_BASE_H

#ifndef NNGCPP_BINARY_MESSAGE_H
        class binary_message;
#endif // NNGCPP_BINARY_MESSAGE_H

        // TODO: so, equal_to is available, but not_equal_to is not?
        class message_pipe : public options_reader, public std::equal_to<message_pipe> {
        protected:

            friend class binary_message;

            ::nng_pipe pid;

        public:

            message_pipe(message_base* const mbp);

            virtual ~message_pipe();

            virtual void close();

            virtual bool has_one() const;

            bool operator==(const message_pipe& rhs);
            // TODO: TBD: so, apparently there is no std::not_equal_to available from the Microsoft implementation, however, we should still be able to override the operator
            bool operator!=(const message_pipe& rhs);

            virtual void get_option(const std::string& name, void* valp, size_type& sz) override;

            virtual void get_option(const std::string& name, std::string& val) override;
            virtual void get_option(const std::string& name, std::string& val, size_type& sz) override;

            virtual void get_option(const std::string& name, address& val);

            virtual void get_option_int(const std::string& name, int& val) override;
            virtual void get_option_sz(const std::string& name, size_type& val) override;

            virtual void get_option(const std::string& name, duration_type& val) override;
            virtual void get_option_ms(const std::string& name, duration_rep_type& val) override;
        };
    }
}

#endif // NNGCPP_MESSAGE_PIPE_H
