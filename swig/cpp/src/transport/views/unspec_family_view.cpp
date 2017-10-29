#include "unspec_family_view.h"

namespace nng {

    _UnspecFamilyView::_UnspecFamilyView(sockaddr_type* const sap)
        : IAddrFamilyViewBase(sap, af_unspec) {
    }

    _UnspecFamilyView::~_UnspecFamilyView() {
    }

    bool _UnspecFamilyView::HasOne() const {
        return false;
    }

    _SockAddrFamilyType _UnspecFamilyView::get_family() const {
        return static_cast<_SockAddrFamilyType>(_sap->s_un.s_family);
    }
}
