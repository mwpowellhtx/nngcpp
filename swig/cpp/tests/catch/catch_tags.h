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

#ifndef NNGCPP_TESTS_CATCH_TAGS_H
#define NNGCPP_TESTS_CATCH_TAGS_H

#include <string>
#include <vector>

namespace Catch {

    template<typename Ty_, typename... Args_>
    std::string Tags(Ty_ x, Args_... others) {
        return Tags(x) + Tags(others...);
    }

    template<typename Ty_>
    std::string Tags(Ty_ x) {
        return Tags(x);
    }

    template<>
    std::string Tags<std::string>(std::string x);

    template<>
    std::string Tags<const char*>(const char* x);

    template<>
    std::string Tags<std::vector<std::string>>(std::vector<std::string> x);
}

#endif // NNGCPP_TESTS_CATCH_TAGS_H
