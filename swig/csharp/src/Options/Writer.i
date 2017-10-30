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
#include "cpp/src/options/writer.h"
%}

%module(directors="1") Options;

%include "typemaps.i"
%include "std_string.i"
%include "csharp/src/Core/Types.i"

namespace nng {

%ignore _BasicOptionWriter;

%typemap(csimports) _OptionWriter %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _OptionWriter "public partial class";
%typemap(csinterfaces) _OptionWriter "IOptionWriter";
%typemap(csbase) _OptionWriter "";

// Ditto Option Reader/Writer.
%ignore _OptionWriter::set(const std::string&, const void*, size_type);
%ignore _OptionWriter::SetDuration(const std::string&, const duration_type&);

%rename("OptionWriter") _OptionWriter;

}

%include "cpp/src/options/writer.h"
