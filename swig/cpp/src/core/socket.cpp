#include <vector>

#include "socket.h"
#include "dialer.h"
#include "listener.h"
#include "nng_exception.h"

namespace nng {

    socket::socket(const nng_ctor_func& nng_ctor)
        : sid(0)
        , sender()
        , receiver()
        , messenger()
        , options() {

        const auto errnum = nng_ctor(&sid);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    socket::~socket() {
        close();
    }

    // TODO: TBD: ditto ec handling...
    void socket::listen(const std::string& addr, int flags) {
        const auto errnum = ::nng_listen(sid, addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::listen(const std::string& addr, listener* const lp, int flags) {
        const auto errnum = ::nng_listen(sid, addr.c_str(), lp ? &(lp->lid) : nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, int flags) {
        const auto errnum = ::nng_dial(sid, addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, dialer* const dp, int flags) {
        const auto errnum = ::nng_dial(sid, addr.c_str(), dp ? &(dp->did) : nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::close() {
        if (sid) {
            auto errnum = ::nng_close(sid);
            THROW_NNG_EXCEPTION_EC(errnum);
            sid = 0;
        }
    }

    void socket::shutdown() {
        if (sid) {
            // TODO: TBD: I'm not sure "shutdown" is quite the same thing as "close". In other words, we may still operate on the ID.
            auto errnum = ::nng_shutdown(sid);
            THROW_NNG_EXCEPTION_EC(errnum);
        }
    }

    template<class Buffer_>
    int send(int id, const Buffer_& buf, std::size_t sz, int flags) {
        const auto errnum = ::nng_send(id, (void*)&buf[0], sz, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }
    
    template<class Buffer_>
    int try_receive(int id, Buffer_& buf, std::size_t& sz, int flags) {
        buf.resize(sz);
        const auto errnum = ::nng_recv(id, &buf[0], &sz, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }

    void socket::send(const binary_message_type* const bmp, int flags) {
        auto* msgp = bmp->get_msgp();
        const auto errnum = ::nng_sendmsg(sid, msgp, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    int socket::send(const buffer_vector_type* const bufp, int flags) {
        return nng::send(sid, *bufp, bufp->size(), flags);
    }

    int socket::send(const buffer_vector_type* const bufp, size_type sz, int flags) {
        return nng::send(sid, *bufp, sz, flags);
    }

    std::unique_ptr<socket::binary_message_type> socket::receive(int flags) {
        auto bmup = std::make_unique<binary_message_type>();
        try_receive(bmup.get(), flags);
        return bmup;
    }

    int socket::try_receive(binary_message_type* const bmp, int flags) {
        ::nng_msg* msgp = nullptr;
        const auto errnum = ::nng_recvmsg(sid, &msgp, flags);
        try {
            THROW_NNG_EXCEPTION_EC(errnum);
        }
        catch (...) {
            // TODO: TBD: this is probably (PROBABLY) about as good as we can expect here...
            if (msgp != nullptr) {
                ::nng_msg_free(msgp);
            }
            // Re-throw the exception after taking care of potential memory allocation.
            throw;
        }
        bmp->set_msgp(msgp);
        return errnum;
    }

    socket::buffer_vector_type socket::receive(size_type& sz, int flags) {
        buffer_vector_type buf;
        try_receive(&buf, sz, flags);
        return buf;
    }

    int socket::try_receive(buffer_vector_type* const bufp, size_type& sz, int flags) {
        return nng::try_receive(sid, *bufp, sz, flags);
    }

    // Convenience option wrappers.
    void socket::set_option(const std::string& name, const std::string& val, option_size_type sz) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, const std::string& val) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, std::string& val, option_size_type& sz) {
        const auto errnum = ::nng_getopt(sid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void socket::get_option(const std::string& name, std::string& val) {
        option_size_type sz = val.size();
        const auto errnum = ::nng_getopt(sid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void socket::set_option(const std::string& name, const void* valp, option_size_type sz) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), valp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, void* val, option_size_type* szp) {
        const auto errnum = ::nng_getopt(sid, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_int(const std::string& name, int val) {
        const auto errnum = ::nng_setopt_int(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_size(const std::string& name, option_size_type val) {
        const auto errnum = ::nng_setopt_size(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_usec(const std::string& name, uint64_t val) {
        const auto errnum = ::nng_setopt_usec(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_int(const std::string& name, int* valp) {
        const auto errnum = ::nng_getopt_int(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_size(const std::string& name, option_size_type* valp) {
        const auto errnum = ::nng_getopt_size(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_usec(const std::string& name, uint64_t* valp) {
        const auto errnum = ::nng_getopt_usec(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    protocol_type to_protocol_type(int value) {
        return static_cast<protocol_type>(value);
    }

    flag_type to_flag_type(int value) {
        return static_cast<flag_type>(value);
    }

    int to_int(const protocol_type value) {
        return static_cast<int>(value);
    }

    int to_int(const flag_type value) {
        return static_cast<int>(value);
    }

    protocol_type socket::get_protocol() const {
        return to_protocol_type(::nng_protocol(sid));
    }

    protocol_type socket::get_peer() const {
        return to_protocol_type(::nng_peer(sid));
    }

    bool socket::is_protocol_configured() const {
        return get_protocol() != protocol_none;
    }

    bool socket::is_peer_configured() const {
        return get_peer() != protocol_none;
    }
}
