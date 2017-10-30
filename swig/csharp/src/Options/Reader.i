/*
  Copyright (c) 2017 Michael W. Powell <mwpowellhtx@gmail.com> All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

%{

/* It may not be necessary to include it, but I did so for clarity's sake.
We do not want to map anything from ADDRESS in this interface, however. */
#include "cpp/src/transport/address.h"

#include "cpp/src/options/reader.h"
%}

%module(directors="1") Options;

%include "typemaps.i"
%include "std_string.i"
%include "csharp/src/Core/Types.i"

namespace nng {

/* We require this little bit of comprehension for _SockAddr. I've wrapped in
two separate namespace blocks to make it clearer what the dependencies are. */

/* So it seems that the key about the base class being dealt with (i.e. removing it in favor
of interfaces only), is that they must be abstract base classes in the C++ API. After which,
SWIG is perfectly happy to remove them from the equation upon request. This issue was exposed
for the combination ReaderWriter.i interface. */

/* TODO: TBD: this bit of surface type mapping may be somewhat naive of me. We'll have to
see whether this works or whether there isn't something deeper going on in the exposed SWIG
code. */

%typemap(cstype) _SockAddr "SocketAddress"

}

namespace nng {

%ignore _BasicOptionReader;

%typemap(csimports) _OptionReader %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _OptionReader "public partial class";
%typemap(csinterfaces) _OptionReader "IOptionReader";
%typemap(csbase) _OptionReader "";

// Ignore any purely C++ facing API that do not easily map into C#.
%ignore _OptionReader::get(const std::string&, void*, size_type&);
%ignore _OptionReader::GetDuration(const std::string&);

/* Yes, an INOUT, or "ref", is more appropriate in this instance because want
to start with a Size value, but are unlikely to end up there afterwards. */

%apply size_type& INOUT { size_type& sz };

%rename("OptionReader") _OptionReader;

}

// We #included "path/to/address.h", but we do not need to %include it.
%include "cpp/src/options/reader.h"
