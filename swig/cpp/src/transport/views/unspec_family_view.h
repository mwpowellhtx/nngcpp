#ifndef NNGCPP_TRANSPORT_VIEWS_UNSPEC_FAMILY_VIEW_H
#define NNGCPP_TRANSPORT_VIEWS_UNSPEC_FAMILY_VIEW_H

#include "IFamilyView.h"

namespace nng {

    struct _UnspecFamilyView : public IAddrFamilyViewBase {

        virtual ~_UnspecFamilyView();

        virtual bool HasOne() const override;

    protected:

        friend void align_view(const _SockAddr&);

        _UnspecFamilyView(sockaddr_type* const sap);
    };
}

#endif // NNGCPP_TRANSPORT_VIEWS_UNSPEC_FAMILY_VIEW_H
