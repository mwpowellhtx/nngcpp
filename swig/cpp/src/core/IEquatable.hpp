#ifndef NNGCPP_IEQUATABLE_HPP
#define NNGCPP_IEQUATABLE_HPP

namespace nng {

    template<typename Ty_>
    struct IEquatable {
        virtual bool Equals(const Ty_&) const = 0;
    };
}

#endif // NNGCPP_IEQUATABLE_HPP
