#include "zt_family_view.h"

namespace nng {

    _ZeroTierFamilyView::_ZeroTierFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_zt) {
    }

    _ZeroTierFamilyView::~_ZeroTierFamilyView() {
    }

    bool _ZeroTierFamilyView::HasOne() const {
        return __family_view_base::HasOne();
    }

    _ZeroTierFamilyView::detail_type* _ZeroTierFamilyView::get_detail() const {
        return &_sap->s_un.s_zt;
    }

    bool _ZeroTierFamilyView::Equals(detail_type* const ap, detail_type* const bp) const {
        return ap == bp || (
            ap->sa_nodeid == bp->sa_nodeid
            && ap->sa_nwid == bp->sa_nwid
            && ap->sa_port == bp->sa_port
            );
    }

    bool _ZeroTierFamilyView::Equals(const IAddrFamilyViewBase& other) const {
        if (!__family_view_base::Equals(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_ZeroTierFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return Equals(get_detail(), op->get_detail());
    }

    bool _ZeroTierFamilyView::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool _ZeroTierFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    nng::uint32_t _ZeroTierFamilyView::GetIPv4Addr() const {
        return __family_view_base::GetIPv4Addr();
    }

    void _ZeroTierFamilyView::SetIPv4Addr(const nng::uint32_t value) {
        __family_view_base::SetIPv4Addr(value);
    }

    IPv6AddrVector _ZeroTierFamilyView::GetIPv6Addr() const {
        return __family_view_base::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _ZeroTierFamilyView::GetIPv6Addr16() const {
        return __family_view_base::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _ZeroTierFamilyView::GetIPv6Addr32() const {
        return __family_view_base::GetIPv6Addr32();
    }

    void _ZeroTierFamilyView::SetIPv6Addr(IPv6AddrVector value) {
        __family_view_base::SetIPv6Addr(value);
    }

    void _ZeroTierFamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        __family_view_base::SetIPv6Addr16(value);
    }

    void _ZeroTierFamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        __family_view_base::SetIPv6Addr32(value);
    }

    nng::uint16_t _ZeroTierFamilyView::__GetPort() const {
        return __family_view_base::__GetPort();
    }

    void _ZeroTierFamilyView::__SetPort(const nng::uint16_t value) {
        __family_view_base::__SetPort(value);
    }
}
