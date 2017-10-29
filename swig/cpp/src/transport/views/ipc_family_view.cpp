#include "ipc_family_view.h"

namespace nng {

    _IpcFamilyView::_IpcFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_ipc) {
    }

    _IpcFamilyView::~_IpcFamilyView() {
    }

    _IpcFamilyView::detail_type* _IpcFamilyView::get_detail() const {
        return &_sap->s_un.s_path;
    }

    bool _IpcFamilyView::operator==(const detail_type& other) {
        return std::memcmp(&(get_detail()->sa_path), &other.sa_path, NNG_MAXADDRLEN) == 0;
    }

    bool _IpcFamilyView::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool _IpcFamilyView::operator==(const IAddrFamilyViewBase& other) {

        if (!IAddrFamilyViewBase::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_IpcFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool _IpcFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    _SockAddrFamilyType _IpcFamilyView::get_family() const {
        return static_cast<_SockAddrFamilyType>(get_detail()->sa_family);
    }
}
