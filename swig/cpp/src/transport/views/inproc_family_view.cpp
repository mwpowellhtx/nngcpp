#include "inproc_family_view.h"

namespace nng {

    _InprocFamilyView::_InprocFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_inproc) {
    }

    _InprocFamilyView::~_InprocFamilyView() {
    }

    _InprocFamilyView::detail_type* _InprocFamilyView::get_detail() const {
        return &_sap->s_un.s_inproc;
    }

    bool _InprocFamilyView::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return std::memcmp(&(detailp->sa_path), &other.sa_path, NNG_MAXADDRLEN) == 0;
    }

    bool _InprocFamilyView::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool _InprocFamilyView::operator==(const IAddrFamilyViewBase& other) {

        if (!IAddrFamilyViewBase::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_InprocFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool _InprocFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    _SockAddrFamilyType _InprocFamilyView::get_family() const {
        return static_cast<_SockAddrFamilyType>(get_detail()->sa_family);
    }
}
