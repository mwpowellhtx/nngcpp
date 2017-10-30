#ifndef NNGCPP_TRANSPORT_ADDRESS_H
#define NNGCPP_TRANSPORT_ADDRESS_H

#include "views/views.h"

#include "../core/equatable.hpp"
#include "../core/having_one.hpp"

#include <cstddef>
#include <memory>

namespace nng {

    template<SocketAddressFamily Val_>
    struct sockaddr_family_name
        : std::integral_constant<SocketAddressFamily, Val_> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_unspec>
        : std::integral_constant<SocketAddressFamily, af_unspec> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inproc>
        : std::integral_constant<SocketAddressFamily, af_inproc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_ipc>
        : std::integral_constant<SocketAddressFamily, af_ipc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet>
        : std::integral_constant<SocketAddressFamily, af_inet> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet6>
        : std::integral_constant<SocketAddressFamily, af_inet6> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_zt>
        : std::integral_constant<SocketAddressFamily, af_zt> {
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
        , public IEquatable<_SockAddr>
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

        nng::uint16_t GetFamily() const;

        void SetFamily(const nng::uint16_t value);

        IAddrFamilyViewBase* const GetView() const;

        virtual bool HasOne() const override;

        virtual bool Equals(const _SockAddr& other) const;

        bool operator==(const _SockAddr& other);

        bool operator!=(const _SockAddr& other);

        static _SockAddr GetIPv4Loopback();

        static _SockAddr GetIPv6Loopback();

        static std::string GetFamilyNameOf(nng::uint16_t value);
    };
}

#endif // NNGCPP_TRANSPORT_ADDRESS_H
