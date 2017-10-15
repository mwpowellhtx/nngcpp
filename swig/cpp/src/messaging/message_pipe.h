#ifndef NNGCPP_MESSAGE_PIPE_H
#define NNGCPP_MESSAGE_PIPE_H

#include <nng/nng.h>

#include "../core/options.h"

#include <algorithm>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_MESSAGE_BASE_H
        class message_base;
#endif // NNGCPP_MESSAGE_BASE_H

#ifndef NNGCPP_BINARY_MESSAGE_H
        class binary_message;
#endif // NNGCPP_BINARY_MESSAGE_H

        // TODO: so, equal_to is available, but not_equal_to is not?
        class message_pipe : public options, public std::equal_to<message_pipe> {
        protected:

            friend class binary_message;

            ::nng_pipe pid;

        public:

            message_pipe(const message_base* const mbp);

            virtual ~message_pipe();

            virtual void close();

            virtual bool has_pipe() const;

            bool operator==(const message_pipe& rhs);
            // TODO: TBD: so, apparently there is no std::not_equal_to available from the Microsoft implementation, however, we should still be able to override the operator
            bool operator!=(const message_pipe& rhs);

            virtual void set_option(const std::string& name, const std::string& val, option_size_type sz) override;
            virtual void set_option(const std::string& name, const std::string& val) override;

            virtual void get_option(const std::string& name, std::string& val) override;
            virtual void get_option(const std::string& name, std::string& val, option_size_type& sz) override;

            virtual void set_option(const std::string& name, const void* valp, option_size_type sz) override;
            virtual void get_option(const std::string& name, void* val, option_size_type* szp) override;

            virtual void set_option_int(const std::string& name, int val) override;
            virtual void set_option_size(const std::string& name, option_size_type val) override;
            virtual void set_option_usec(const std::string& name, option_ulonglong_type val) override;

            virtual void get_option_int(const std::string& name, int* valp) override;
            virtual void get_option_size(const std::string& name, option_size_type* valp) override;
            virtual void get_option_usec(const std::string& name, option_ulonglong_type* valp) override;
        };
    }
}

#endif // NNGCPP_MESSAGE_PIPE_H
