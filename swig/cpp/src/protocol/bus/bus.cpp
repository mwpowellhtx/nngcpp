#include "bus.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_bus_open, I think it is sufficient to just use the versioned symbol.
            _BusSocket::_BusSocket() : _Socket(std::bind(&(::nng_bus0_open), _1)) {
            }

            _BusSocket::~_BusSocket() {
            }

            _OptionReaderWriter* const _BusSocket::GetOptions() {
                return _Socket::GetOptions();
            }

            // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
            void _BusSocket::Listen(const std::string& addr, flag_type flags) {
                _Socket::Listen(addr, flags);
            }

            void _BusSocket::Listen(const std::string& addr, _Listener* const l, flag_type flags) {
                _Socket::Listen(addr, l, flags);
            }

            void _BusSocket::Dial(const std::string& addr, flag_type flags) {
                _Socket::Dial(addr, flags);
            }

            void _BusSocket::Dial(const std::string& addr, _Dialer* const d, flag_type flags) {
                _Socket::Dial(addr, d, flags);
            }

            void _BusSocket::Close() {
                _Socket::Close();
            }

            bool _BusSocket::HasOne() const {
                return _Socket::HasOne();
            }

            void _BusSocket::Send(binary_message& m, flag_type flags) {
                _Socket::Send(m, flags);
            }

            void _BusSocket::Send(const buffer_vector_type& buf, flag_type flags) {
                _Socket::Send(buf, flags);
            }

            void _BusSocket::Send(const buffer_vector_type& buf, size_type sz, flag_type flags) {
                _Socket::Send(buf, sz, flags);
            }

            void _BusSocket::SendAsync(const basic_async_service* const svcp) {
                _Socket::SendAsync(svcp);
            }
        }
    }
}
