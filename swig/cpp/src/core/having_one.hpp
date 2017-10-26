#ifndef NNGCPP_HAVING_ONE_HPP
#define NNGCPP_HAVING_ONE_HPP

namespace nng {

    struct IHaveOne {
        virtual bool HasOne() const = 0;
    };
}

#endif // NNGCPP_HAVING_ONE_HPP
