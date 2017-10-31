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
#include "cpp/src/core/dialer.h"
%}

%module(directors="1") Core;

%include "typemaps.i"
%include "std_string.i"
%include "csharp/src/Core/Types.i"

namespace nng {

%typemap(cstype) const socket& "Socket"
%typemap(cstype) _OptionReaderWriter* const "OptionReaderWriter"

}

namespace nng {

%typemap(csimports) _Dialer %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _Dialer "public partial class";
%typemap(csbase) _Dialer "";
%typemap(csinterfaces) _Dialer "IHaveOne, ICanClose, IDialer, IEndPoint, IHaveOptionReaderWriter";
%typemap(cscode) _Dialer %{
  public virtual IOptionReaderWriter Options {
    get { return GetOptions(); }
  }
%}

%rename("Dialer") _Dialer;

}

%include "cpp/src/core/dialer.h"
