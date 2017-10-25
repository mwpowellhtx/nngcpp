#include "message_pipe.h"
#include "message_base.h"
#include "../core/invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    message_pipe::nng_type get_id(const message_pipe& mp) {
        return mp.pid;
    }

    message_pipe::message_pipe(msg_type* const msgp)
        : having_one(), can_close()
        , pid(0), _msgp(msgp), _options()
        , __getter(bind(&::nng_msg_get_pipe, _msgp))
        , __setter(bind(&::nng_msg_set_pipe, _msgp, pid))
        , __closer(bind(&::nng_pipe_close, pid)) {

        //configure(msgp);
        invocation::with_result(__getter, &pid);
        configure(pid, _msgp);
    }

    message_pipe::message_pipe(message_base* const mbp)
        : having_one(), can_close()
        , pid(0), _msgp(get_msgp(mbp)), _options()
        , __getter(bind(&::nng_msg_get_pipe, _msgp))
        , __setter(bind(&::nng_msg_set_pipe, _msgp, pid))
        , __closer(bind(&::nng_pipe_close, pid)) {

        // TODO: TBD: throw an exception upon irregular pipe value...
        //configure(msgp);
        invocation::with_result(__getter, &pid);
        configure(pid, _msgp);
    }

    // TODO: TBD: this is looking very similar to socket. perhaps it is...
    message_pipe::~message_pipe() {
        close();
    }

    void message_pipe::close() {
        if (!has_one()) { return; }
        // TODO: TBD: should be fine trapping non-ec_enone RV's only; but consider whether ec_enoent was appropriate...
        invocation::with_error_handling_if_not_one_of(__closer, { ec_enone, ec_enoent });
        configure(pid = 0);
    }

    bool message_pipe::has_one() const {
        return pid > 0;
    }

    void message_pipe::reset() {
        invocation::with_void_return_value(__setter);
        /* TODO: TBD: once the pipe has been returned to the originating Message Base,
        I believe the semantics means we must consider the pipe invalidated. */
        configure(pid = 0, _msgp = nullptr);
    }

    //void message_pipe::configure(msg_type* msgp) {
    //    __getter = bind(&::nng_msg_get_pipe, msgp);
    //}

    void message_pipe::configure(nng_type pid, msg_type* msgp) {

        __setter = bind(&::nng_msg_set_pipe, msgp, pid);
        __closer = bind(&::nng_pipe_close, pid);

        _options.set_getters(
            bind(&::nng_pipe_getopt, pid, _1, _2, _3)
            , bind(&::nng_pipe_getopt_int, pid, _1, _2)
            , bind(&::nng_pipe_getopt_size, pid, _1, _2)
            , bind(&::nng_pipe_getopt_ms, pid, _1, _2)
        );
    }

    options_reader* const message_pipe::options() {
        return &_options;
    }

    bool message_pipe::operator==(const message_pipe& rhs) {
        return pid == rhs.pid;
    }

    bool message_pipe::operator!=(const message_pipe& rhs) {
        return pid != rhs.pid;
    }
}
