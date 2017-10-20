#include "message_pipe.h"
#include "message_base.h"
#include "../core/exceptions.hpp"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;
        using std::placeholders::_4;

        message_pipe::message_pipe(message_base* const mbp)
            : options_reader()
            , pid(0) {

            // TODO: TBD: throw an exception upon irregular pipe value...
            const auto& op = std::bind(&::nng_msg_get_pipe, _1);
            pid = op(mbp->get_msgp());
        }

        // TODO: TBD: this is looking very similar to socket. perhaps it is...
        message_pipe::~message_pipe() {
            close();
        }

        void message_pipe::close() {
            if (!has_one()) { return; }
            using std::placeholders::_1;
            const auto op = std::bind(::nng_pipe_close, _1);
            const auto errnum = op(pid);
            THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone, ec_enoent);
            pid = 0;
        }

        bool message_pipe::has_one() const {
            return pid > 0;
        }

        bool message_pipe::operator==(const message_pipe& rhs) {
            return pid == rhs.pid;
        }

        bool message_pipe::operator!=(const message_pipe& rhs) {
            return pid != rhs.pid;
        }

        void message_pipe::get_option(const std::string& name, void* val, size_type* szp) {
            const auto& op = std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), val, szp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option(const std::string& name, std::string& val) {
            size_type sz = val.size();
            const auto& op = std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), (void*)val.data(), &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            //// Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            //val.resize(sz - 1);
        }

        void message_pipe::get_option(const std::string& name, std::string& val, size_type& sz) {
            const auto& op =  std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), (void*)&val[0], &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            //// Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            //val.resize(sz - 1);
        }

        void message_pipe::get_option_int(const std::string& name, int* valp) {
            const auto& op = std::bind(::nng_pipe_getopt_int, _1, _2, _3);
            const auto errnum = op(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_size(const std::string& name, size_type* valp) {
            const auto& op = std::bind(::nng_pipe_getopt_size, _1, _2, _3);
            const auto errnum = ::nng_getopt_size(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option(const std::string& name, duration_type* valp) {
            duration_type::rep val;
            get_option_ms(name, &val);
            *valp = duration_type(val);
        }

        void message_pipe::get_option_ms(const std::string& name, duration_rep_type* valp) {
            //throw trx::not_implemented("::nng_pipe_getopt_usec missing from the nng source");
            // TODO: TBD: see repo issue: http://github.com/nanomsg/nng/issues/116
            const auto& op = std::bind(::nng_pipe_getopt_ms, _1, _2, _3);
            const auto errnum = op(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }
    }
}
