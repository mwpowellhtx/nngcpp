#include "ipc_family_view.h"

namespace nng {

    _IpcFamilyView::_IpcFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_ipc) {
    }

    _IpcFamilyView::~_IpcFamilyView() {
    }

    bool _IpcFamilyView::HasOne() const {
        return __family_view_base::HasOne();
    }

    _IpcFamilyView::detail_type* _IpcFamilyView::get_detail() const {
        return &_sap->s_un.s_path;
    }

    bool _IpcFamilyView::Equals(detail_type* const ap, detail_type* const bp) const {
        // TODO: TBD: expose Get/Set-Path members across the views...
        return std::memcmp(&(ap->sa_path), &(bp->sa_path), NNG_MAXADDRLEN) == 0;
    }

    bool _IpcFamilyView::Equals(const IAddrFamilyViewBase& other) const {
        if (!__family_view_base::Equals(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_IpcFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other));

        if (!op) { return false; }

        return Equals(get_detail(), op->get_detail());
    }

    bool _IpcFamilyView::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool _IpcFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    nng::uint32_t _IpcFamilyView::GetIPv4Addr() const {
        return __family_view_base::GetIPv4Addr();
    }

    void _IpcFamilyView::SetIPv4Addr(const nng::uint32_t value) {
        __family_view_base::SetIPv4Addr(value);
    }

    IPv6AddrVector _IpcFamilyView::GetIPv6Addr() const {
        return __family_view_base::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _IpcFamilyView::GetIPv6Addr16() const {
        return __family_view_base::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _IpcFamilyView::GetIPv6Addr32() const {
        return __family_view_base::GetIPv6Addr32();
    }

    void _IpcFamilyView::SetIPv6Addr(IPv6AddrVector value) {
        __family_view_base::SetIPv6Addr(value);
    }

    void _IpcFamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        __family_view_base::SetIPv6Addr16(value);
    }

    void _IpcFamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        __family_view_base::SetIPv6Addr32(value);
    }

    nng::uint16_t _IpcFamilyView::__GetPort() const {
        return __family_view_base::__GetPort();
    }

    void _IpcFamilyView::__SetPort(const nng::uint16_t value) {
        __family_view_base::__SetPort(value);
    }
}
