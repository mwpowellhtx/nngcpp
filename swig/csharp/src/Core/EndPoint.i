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
#include "cpp/src/core/endpoint.h"
%}

%module(directors="1") Core;

%include "typemaps.i"
%include "csharp/src/Core/Types.i"

namespace nng {

%typemap(cstype) _OptionReaderWriter* const "OptionReaderWriter"

}

namespace nng {

// TODO: TBD: we may just "ignore" the _EndPoint itself as an abstract base class. No need to expose it directly apart from a complementary interface.
%typemap(csimports) _EndPoint %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _EndPoint "public abstract partial class";
%typemap(csbase) _EndPoint "";
%typemap(csinterfaces) _EndPoint "IHaveOne, ICanClose, IEndPoint, IHaveOptionReaderWriter";
%typemap(cscode) _EndPoint %{
  public abstract void Close();

  public abstract bool HasOne();

  public virtual IOptionReaderWriter Options {
    get { return GetOptions(); }
  }
%}

%rename("EndPoint") _EndPoint;

}

%include "cpp/src/core/endpoint.h"
