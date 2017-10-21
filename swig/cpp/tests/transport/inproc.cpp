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

#include "transport.h"

#include "../catch/catch_tags.h"

namespace constants {

    const std::vector<std::string> prefix_tags = { "inproc" };

    const std::string test_addr_base = "inproc://test_";

    const char port_delim = '\0';
}

void init(const std::string& addr) {}
