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

#include "catch_tags.h"

#include <sstream>
#include <algorithm>

namespace Catch {

    template<>
    std::string Tags<std::string>(std::string x) {
        std::ostringstream os;
        os << "[" << x << "]";
        return os.str();
    }

    template<>
    std::string Tags<const char*>(const char* x) {
        return Tags(std::string(x));
    }

    template<>
    std::string Tags<std::vector<std::string>>(std::vector<std::string> x) {
        std::string result;
        std::for_each(x.cbegin(), x.cend(), [&result](const std::string& y) {
            result += Tags(y);
        });
        return result;
    }
}
