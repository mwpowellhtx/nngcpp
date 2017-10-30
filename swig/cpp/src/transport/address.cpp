#include "address.h"

#include "../core/exceptions.hpp"

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

#include <sstream>
#include <cstring>

#define SOCKADDR_FAMILY_TO_STRING(x) #x

#define THROW_VIEW_NOT_IMPLEMENTED_EX(name) throw nng::exceptions::not_implemented(#name " view is not implemented for the address family");

namespace nng {

    template<SocketAddressFamily Val_>
    const std::string sockaddr_family_name<Val_>::name = "";

    const std::string sockaddr_family_name<af_unspec>::name = SOCKADDR_FAMILY_TO_STRING(af_unspec);
    const std::string sockaddr_family_name<af_inproc>::name = SOCKADDR_FAMILY_TO_STRING(af_inproc);
    const std::string sockaddr_family_name<af_ipc>::name = SOCKADDR_FAMILY_TO_STRING(af_ipc);
    const std::string sockaddr_family_name<af_inet>::name = SOCKADDR_FAMILY_TO_STRING(af_inet);
    const std::string sockaddr_family_name<af_inet6>::name = SOCKADDR_FAMILY_TO_STRING(af_inet6);
    const std::string sockaddr_family_name<af_zt>::name = SOCKADDR_FAMILY_TO_STRING(af_zt);

    const std::string _SockAddrFamilyNames::unspec = sockaddr_family_name<af_unspec>::name;
    const std::string _SockAddrFamilyNames::inproc = sockaddr_family_name<af_inproc>::name;
    const std::string _SockAddrFamilyNames::ipc = sockaddr_family_name<af_ipc>::name;
    const std::string _SockAddrFamilyNames::inet = sockaddr_family_name<af_inet>::name;
    const std::string _SockAddrFamilyNames::inet6 = sockaddr_family_name<af_inet6>::name;
    const std::string _SockAddrFamilyNames::zt = sockaddr_family_name<af_zt>::name;

    _SockAddrFamilyNames::_SockAddrFamilyNames() {}

    _SockAddr::_SockAddr() : _sa(), _view() {
        // TODO: TBD: this is probably as safe an assumption as possible...
        std::memset(&_sa, 0, sizeof(_sa));
    }

    _SockAddr::_SockAddr(const _SockAddr& other) : _sa(), _view() {
        (*this) = other;
    }

    _SockAddr::~_SockAddr() {
    }

    _SockAddr& _SockAddr::operator=(const _SockAddr& other) {
        std::memcpy(&_sa, &other._sa, sizeof(_sa));
        return *this;
    }

    std::string _SockAddr::GetFamilyNameOf(uint16_t value) {
        switch (value) {
        case af_inproc: return sockaddr_family_name<af_inproc>::name;
        case af_ipc: return sockaddr_family_name<af_ipc>::name;
        case af_inet: return sockaddr_family_name<af_inet>::name;
        case af_inet6: return sockaddr_family_name<af_inet6>::name;
        case af_zt: return sockaddr_family_name<af_zt>::name;
        case af_unspec: return sockaddr_family_name<af_unspec>::name;
        }

        std::ostringstream os;
        os << "unknown family type (" << value << ")";
        return os.str();
    }

    void align_view(const _SockAddr& _Address) {

        // Do the const cast dance to thread the needle.
        auto& a = const_cast<_SockAddr&>(_Address);

        auto& _sa = a._sa;
        auto& _view = a._view;

        // Should be safe to do this regardless of the state of the union.
        const auto actual_jewel = _sa.s_un.s_family;

        if (_view != nullptr && _view->GetJewel() == actual_jewel) {
            return;
        }

        // For use regardless of the intended view.
        const auto sap = a.get();

        // Trusting that Jewel is not somehow otherwise accidentally correct!
        switch (actual_jewel) {
        case af_inproc:
            _view.reset(new _InprocFamilyView(sap));
            break;
        case af_ipc:
            _view.reset(new _IpcFamilyView(sap));
            break;
        case af_inet:
            _view.reset(new _InetFamilyView(sap));
            break;
        case af_inet6:
            _view.reset(new _Inet6FamilyView(sap));
            break;
        case af_zt:
            _view.reset(new _ZeroTierFamilyView(sap));
            break;
        case af_unspec:
        default:
            _view.reset(new _UnspecFamilyView(sap));
            break;
        }
    }

    IAddrFamilyViewBase* const _SockAddr::GetView() const {
        align_view(*this);
        return _view.get();
    }

    bool _SockAddr::HasOne() const {
        return GetView() != nullptr;
    }

    size_type _SockAddr::GetSize() const {
        return sizeof(_sa);
    }

    _SockAddr::sockaddr_type* _SockAddr::get() {
        return &_sa;
    }

    nng::uint16_t _SockAddr::GetFamily() const {
        return _sa.s_un.s_family;
    }

    void _SockAddr::SetFamily(const nng::uint16_t value) {
        _sa.s_un.s_family = value;
    }

    bool _SockAddr::Equals(const _SockAddr& other) const {
        // Return early when the Families are different.
        if (GetFamily() != other.GetFamily()) { return false; }

        // We must cast the constness out of the other for this operation.
        auto otherp = const_cast<_SockAddr*>(&other);

        // We must take this Long Way Round on account of NNG treating the misaligned data as indeterminate.
        return GetView()->Equals(*other.GetView());
    }

    bool _SockAddr::operator==(const _SockAddr& other) {
        return Equals(other);
    }

    bool _SockAddr::operator!=(const _SockAddr& other) {
        return !Equals(other);
    }

    _SockAddr _SockAddr::GetIPv4Loopback() {
        _SockAddr addr;
        addr.SetFamily(af_inet);
        auto vp = addr.GetView();
        vp->SetIPv4Addr(INADDR_LOOPBACK);
        return addr;
    }

    _SockAddr _SockAddr::GetIPv6Loopback() {
        _SockAddr addr;
        addr.SetFamily(af_inet6);
        auto vp = addr.GetView();
        // IPv6 loopback is quite simple: 0000:0000:0000:0000:0000:0000:0000:0001, or simply ::1.
        IPv6AddrVector value = { 1 };
        for (IPv6AddrVector::const_iterator it = value.cbegin(); it != value.cend(); ++it) {

            const auto delta = it - value.cbegin();
            const auto i = sizeof(addr._sa.s_un.s_in6.sa_addr) - delta - 1;
            addr._sa.s_un.s_in6.sa_addr[i] = *it;
        }
        return addr;
    }
}
