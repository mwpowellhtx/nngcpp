#ifndef NNGCPP_ADDRESS_H
#define NNGCPP_ADDRESS_H

#define NNG_ONLY
#include <nngcpp.h>

#include "../nngcpp_integration.h"

#include <cstddef>
#include <vector>
#include <memory>

namespace nng {

    enum sockaddr_family_type : uint16_t {
        af_unspec = ::NNG_AF_UNSPEC,
        af_inproc = ::NNG_AF_INPROC,
        af_ipc = ::NNG_AF_IPC,
        af_inet = ::NNG_AF_INET,
        af_inet6 = ::NNG_AF_INET6,
        af_zt = ::NNG_AF_ZT,  // ZeroTier
    };

    struct family_view_base {

        virtual ~family_view_base();

        virtual sockaddr_family_type get_jewel() const;

        virtual bool has_one() const;

    protected:

        ::nng_sockaddr* _sap;

        family_view_base(::nng_sockaddr* const sap, sockaddr_family_type jewel);

    private:

        const sockaddr_family_type _jewel;

    public:

        typedef std::vector<uint8_t> in6_addr_vector_type;

        virtual uint32_t get_addr() const;

        virtual in6_addr_vector_type get_in6_addr() const;

        virtual uint16_t get_port() const;
    };

    template<sockaddr_family_type Type_, class Detail_>
    struct family_view : public family_view_base {

        virtual ~family_view();

        virtual sockaddr_family_type get_type() const;

        typedef sockaddr_family_type family_type;

        typedef family_view<Type_, Detail_> family_view_type;

        typedef Detail_ detail_type;

    protected:

        family_view(::nng_sockaddr* const sap);

        virtual detail_type* get_detail() const = 0;
    };

    struct unspec_family_view : public family_view_base {

        unspec_family_view(::nng_sockaddr* const sap);

        virtual ~unspec_family_view();
    };

    struct ipc_family_view : public family_view<af_ipc, ::nng_sockaddr_ipc> {

        ipc_family_view(::nng_sockaddr* const sap);

        virtual ~ipc_family_view();

    protected:

        virtual ::nng_sockaddr_ipc* get_detail() const override;
    };

    struct inproc_family_view : public family_view<af_inproc, ::nng_sockaddr_inproc> {

        inproc_family_view(::nng_sockaddr* const sap);

        virtual ~inproc_family_view();

    protected:

        virtual ::nng_sockaddr_inproc* get_detail() const override;
    };

    struct inet_family_view : public family_view<af_inet, ::nng_sockaddr_in> {

        inet_family_view(::nng_sockaddr* const sap);

        virtual ~inet_family_view();

    protected:

        virtual ::nng_sockaddr_in* get_detail() const override;

    public:

        virtual uint32_t get_addr() const override;

        virtual uint16_t get_port() const override;
    };

    struct inet6_family_view : public family_view<af_inet6, ::nng_sockaddr_in6> {

        inet6_family_view(::nng_sockaddr* const sap);

        virtual ~inet6_family_view();

    protected:

        virtual ::nng_sockaddr_in6* get_detail() const override;

        in6_addr_vector_type get_in6_addr() const override;
            
        virtual uint16_t get_port() const override;
    };

    struct zt_family_view : public family_view<af_zt, ::nng_sockaddr_zt> {

        zt_family_view(::nng_sockaddr* const sap);

        virtual ~zt_family_view();

    protected:

        virtual ::nng_sockaddr_zt* get_detail() const override;
    };

    class address {
    public:

        typedef ::nng_sockaddr sockaddr_type;
        typedef  std::size_t size_type;

    private:

        sockaddr_type _sa;

        std::unique_ptr<family_view_base> _view;

        void align_view();

    public:

        address();

        virtual ~address();

        bool has_one();

        size_type get_size() const;

        sockaddr_type* get();

        sockaddr_family_type get_family() const;

        family_view_base* const view();
    };
}

#endif // NNGCPP_ADDRESS_H
