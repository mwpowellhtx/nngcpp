#include "inproc_family_view.h"

namespace nng {

    _InprocFamilyView::_InprocFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_inproc) {
    }

    _InprocFamilyView::~_InprocFamilyView() {
    }

    bool _InprocFamilyView::HasOne() const {
        return __family_view_base::HasOne();
    }

    _InprocFamilyView::detail_type* _InprocFamilyView::get_detail() const {
        return &_sap->s_un.s_inproc;
    }

    bool _InprocFamilyView::Equals(detail_type* const ap, detail_type* const bp) const {
        // TODO: TBD: expose Get/Set-Path members across the views...
        return std::memcmp(&(ap->sa_path), &(bp->sa_path), NNG_MAXADDRLEN) == 0;
    }

    // Fill in the gaps, even calling to base functionality, in order for SWIG to see them properly.
    bool _InprocFamilyView::Equals(const IAddrFamilyViewBase& other) const {
        if (!__family_view_base::Equals(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_InprocFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other));

        if (!op) { return false; }

        return Equals(get_detail(), op->get_detail());
    }

    bool _InprocFamilyView::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool _InprocFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    nng::uint32_t _InprocFamilyView::GetIPv4Addr() const {
        return __family_view_base::GetIPv4Addr();
    }

    void _InprocFamilyView::SetIPv4Addr(const nng::uint32_t value) {
        __family_view_base::SetIPv4Addr(value);
    }

    IPv6AddrVector _InprocFamilyView::GetIPv6Addr() const {
        return __family_view_base::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _InprocFamilyView::GetIPv6Addr16() const {
        return __family_view_base::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _InprocFamilyView::GetIPv6Addr32() const {
        return __family_view_base::GetIPv6Addr32();
    }

    void _InprocFamilyView::SetIPv6Addr(IPv6AddrVector value) {
        __family_view_base::SetIPv6Addr(value);
    }

    void _InprocFamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        __family_view_base::SetIPv6Addr16(value);
    }

    void _InprocFamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        __family_view_base::SetIPv6Addr32(value);
    }

    nng::uint16_t _InprocFamilyView::__GetPort() const {
        return __family_view_base::__GetPort();
    }

    void _InprocFamilyView::__SetPort(const nng::uint16_t value) {
        __family_view_base::__SetPort(value);
    }
}
