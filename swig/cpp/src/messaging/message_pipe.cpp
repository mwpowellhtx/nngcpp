#include "message_pipe.h"
#include "message_base.h"
#include "../core/address.h"
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

        void message_pipe::get_option(const std::string& name, void* valp, size_type& sz) {
            const auto& op = std::bind(&::nng_pipe_getopt, pid, _1, _2, _3);
            options_reader::get_option(op, name, valp, sz);
        }

        void message_pipe::get_option(const std::string& name, std::string& val, size_type& sz) {
            val.resize(sz);
            get_option(name, val);
        }

        void message_pipe::get_option(const std::string& name, address& val) {
            auto sz = val.get_size();
            get_option(name, (void*)val.get(), sz);
        }

        void message_pipe::get_option(const std::string& name, std::string& val) {
            const auto& op = std::bind(::nng_pipe_getopt, pid, _1, _2, _3);
            options_reader::get_option(op, name, val);
        }

        void message_pipe::get_option_int(const std::string& name, int& val) {
            const auto& op = std::bind(::nng_pipe_getopt_int, pid, _1, _2);
            options_reader::get_option_int(op, name, val);
        }

        void message_pipe::get_option_sz(const std::string& name, size_type& val) {
            const auto& op = std::bind(::nng_pipe_getopt_size, pid, _1, _2);
            options_reader::get_option_sz(op, name, val);
        }

        void message_pipe::get_option(const std::string& name, duration_type& val) {
            duration_type::rep x;
            get_option_ms(name, x);
            val = duration_type(x);
        }

        void message_pipe::get_option_ms(const std::string& name, duration_rep_type& val) {
            const auto& op = std::bind(::nng_pipe_getopt_ms, pid, _1, _2);
            options_reader::get_option_ms(op, name.c_str(), val);
        }
    }
}
