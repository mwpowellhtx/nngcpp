#include "zt_family_view.h"

namespace nng {

    _ZeroTierFamilyView::_ZeroTierFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_zt) {
    }

    _ZeroTierFamilyView::~_ZeroTierFamilyView() {
    }

    _ZeroTierFamilyView::detail_type* _ZeroTierFamilyView::get_detail() const {
        return &_sap->s_un.s_zt;
    }

    bool _ZeroTierFamilyView::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return detailp->sa_nodeid == other.sa_nodeid
            && detailp->sa_nwid == other.sa_nwid
            && detailp->sa_port == other.sa_port;
    }

    bool _ZeroTierFamilyView::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool _ZeroTierFamilyView::operator==(const IAddrFamilyViewBase& other) {

        if (!IAddrFamilyViewBase::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_ZeroTierFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool _ZeroTierFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    _SockAddrFamilyType _ZeroTierFamilyView::get_family() const {
        return static_cast<_SockAddrFamilyType>(get_detail()->sa_family);
    }
}
