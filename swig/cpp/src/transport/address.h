#ifndef NNGCPP_TRANSPORT_ADDRESS_H
#define NNGCPP_TRANSPORT_ADDRESS_H

#include "views/views.h"

#include "../core/having_one.hpp"

#include <cstddef>
#include <memory>

namespace nng {

    template<_SockAddrFamilyType Val_>
    struct sockaddr_family_name
        : std::integral_constant<_SockAddrFamilyType, Val_> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_unspec>
        : std::integral_constant<_SockAddrFamilyType, af_unspec> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inproc>
        : std::integral_constant<_SockAddrFamilyType, af_inproc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_ipc>
        : std::integral_constant<_SockAddrFamilyType, af_ipc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet>
        : std::integral_constant<_SockAddrFamilyType, af_inet> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet6>
        : std::integral_constant<_SockAddrFamilyType, af_inet6> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_zt>
        : std::integral_constant<_SockAddrFamilyType, af_zt> {
        static const std::string name;
    };

    struct _SockAddrFamilyNames {

        static const std::string unspec;
        static const std::string inproc;
        static const std::string ipc;
        static const std::string inet;
        static const std::string inet6;
        static const std::string zt;

    private:

        _SockAddrFamilyNames();
    };

    class _SockAddr
        : public IHaveOne
        , public std::equal_to<_SockAddr> {
    public:

        //typedef ::nng_sockaddr sockaddr_type;
        typedef sockaddr_t sockaddr_type;

    private:

        sockaddr_type _sa;

        std::unique_ptr<IAddrFamilyViewBase> _view;

        friend void align_view(const _SockAddr& _Address);

    public:

        _SockAddr();

        _SockAddr(const _SockAddr& other);

        virtual ~_SockAddr();

        size_type GetSize() const;

        sockaddr_type* get();

        _SockAddrFamilyType get_family() const;

        void set_family(const _SockAddrFamilyType value);

        IAddrFamilyViewBase* const view() const;

        virtual bool HasOne() const override;

        bool operator==(const _SockAddr& other);

        bool operator!=(const _SockAddr& other);

        static _SockAddr in_loopback();

        static _SockAddr in6_loopback();

        static std::string name_of(nng::uint16_t value);
    };
}

#endif // NNGCPP_TRANSPORT_ADDRESS_H
