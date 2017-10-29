#ifndef NNGCPP_TRANSPORT_VIEWS_ZT_FAMILY_VIEW_H
#define NNGCPP_TRANSPORT_VIEWS_ZT_FAMILY_VIEW_H

#include "IFamilyView.h"

namespace nng {

    struct _ZeroTierFamilyView : public IAddrFamilyView<sockaddr_zt_t> {

        virtual ~_ZeroTierFamilyView();

        virtual bool operator==(const IAddrFamilyViewBase& other) override;
        virtual bool operator!=(const IAddrFamilyViewBase& other) override;

    protected:

        friend void align_view(const _SockAddr&);

        _ZeroTierFamilyView(sockaddr_type* const sap);

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;
    };
}

#endif // NNGCPP_TRANSPORT_VIEWS_ZT_FAMILY_VIEW_H
