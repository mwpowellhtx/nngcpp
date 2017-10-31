#include "IProtocol.h"

namespace nng {

    IProtocol::IProtocol()
        : _protocol_getter(), _peer_getter() {
    }

    IProtocol::~IProtocol() {
    }

    void IProtocol::set_getters(
        const get_protocol_func& protocol_getter
        , const get_protocol_func& peer_getter) {

        // Make sure that we can (re-)set the internal (const) Getter functions.
        const_cast<get_protocol_func&>(_protocol_getter) = protocol_getter;
        const_cast<get_protocol_func&>(_peer_getter) = peer_getter;
    }

    protocol_type to_protocol_type(nng::uint16_t value) {
        return static_cast<protocol_type>(value);
    }

    protocol_type IProtocol::get_protocol() const {
        return to_protocol_type(_protocol_getter());
    }

    protocol_type IProtocol::get_peer() const {
        return to_protocol_type(_peer_getter());
    }
}
