#ifndef NNGCPP_PROTOCOL_H
#define NNGCPP_PROTOCOL_H

#include "enums.h"

#include <functional>

namespace nng {

    class IProtocol {
    protected:

        typedef std::function<nng::uint16_t()> get_protocol_func;

        IProtocol();

        virtual void set_enabled(bool state);

        virtual void set_getters(
            const get_protocol_func& protocol_getter
            , const get_protocol_func& peer_getter);

        virtual protocol_type get_protocol() const;
        virtual protocol_type get_peer() const;

    private:

        bool _enabled;

        const get_protocol_func _protocol_getter;
        const get_protocol_func _peer_getter;

        void throw_if_not_enabled() const;

    public:

        virtual ~IProtocol();
    };
}

#endif // NNGCPP_PROTOCOL_H
