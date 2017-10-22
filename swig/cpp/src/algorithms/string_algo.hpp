//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
// Copyright 2017 Garrett D'Amore <garrett@damore.org>
// Copyright 2017 Capitar IT Group BV <info@capitar.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_ALGORITHMS_STRING_ALGO_HPP
#define NNGCPP_ALGORITHMS_STRING_ALGO_HPP

#include <locale>
#include <cctype>
#include <algorithm> 

namespace trx {

    // Slightly tweaked from SO: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring#answer-217605

    // trim from start (in place)
    static inline void ltrim(std::string& s) {
        auto found_= std::find_if(s.begin(), s.end(), [](std::string::value_type ch) {
            return ch && !std::isspace(ch);
        });
        s.erase(s.begin(), found_);
    }

    // trim from end (in place)
    static inline void rtrim(std::string& s) {
        auto found_ = std::find_if(s.rbegin(), s.rend(), [](std::string::value_type ch) {
            return ch && !std::isspace(ch);
        });
        s.erase(found_.base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrimcp(const std::string& s) {
        std::string x(s.begin(), s.end());
        ltrim(x);
        return x;
    }

    // trim from end (copying)
    static inline std::string rtrimcp(const std::string& s) {
        std::string x(s.begin(), s.end());
        rtrim(x);
        return x;
    }

    // trim from both ends (copying)
    static inline std::string trimcp(const std::string& s) {
        std::string x(s.begin(), s.end());
        trim(x);
        return x;
    }
}

#endif // NNGCPP_ALGORITHMS_STRING_ALGO_HPP
