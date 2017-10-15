//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_CATCH_EXCEPTION_TRANSLATIONS_HPP
#define NNGCPP_CATCH_EXCEPTION_TRANSLATIONS_HPP

#include <catch.hpp>

#include <core/exceptions.hpp>

#ifndef STRINGIFY
#define STRINGIFY(x)    #x
#endif // STRINGIFY

/* Translating from std::exception parlance, we have:
http://www.cplusplus.com/reference/exception/exception/what/ */
CATCH_TRANSLATE_EXCEPTION(std::exception& ex) {
    const auto ex_what_ = std::string(ex.what());
    return ex_what_.empty()
        ? "unknown " STRINGIFY(std::exception)
        : ("std::exception: " + ex_what_).c_str();
}

CATCH_TRANSLATE_EXCEPTION(nng::nng_exception& ex) {
    // TODO: TBD: consider making this a part of the nng::nng_exception::what method...
    std::ostringstream os;
    const auto errnum = static_cast<int>(ex.error_code);
    os << STRINGIFY(nng::nng_exception) << ": " << ex.what() << " (error_code = " << errnum << ")";
    return os.str();
}

CATCH_TRANSLATE_EXCEPTION(trx::not_implemented& ex) {
    const auto ex_what_ = std::string(ex.what());
    return ex_what_.empty()
        ? "unknown " STRINGIFY(trx::not_implemented)
        : ("trx::not_implemented: " + ex_what_).c_str();
}

CATCH_TRANSLATE_EXCEPTION(trx::invalid_operation& ex) {
    const auto ex_what_ = std::string(ex.what());
    return ex_what_.empty()
        ? "unknown " STRINGIFY(trx::invalid_operation)
        : ("trx::invalid_operation: " + ex_what_).c_str();
}

#endif // NNGCPP_CATCH_EXCEPTION_TRANSLATIONS_HPP
