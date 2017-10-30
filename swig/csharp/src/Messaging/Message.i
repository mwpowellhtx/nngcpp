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
#include "cpp/src/messaging/basic_binary_message.hpp"
#include "cpp/src/messaging/binary_message.h"
%}

%module(directors="1") Messaging;

namespace nng {

%typemap(csimports) _Message %{
using System;
using System.Runtime.InteropServices;
%}

%rename("Message") _Message;

%typemap(csclassmodifiers) _Message "public partial class"
//%typemap(csbase) _Message "Message"
%typemap(csinterfaces) _Message "IHaveOne, IClearable"

%ignore _Message::_Message(msg_type*);

// Add a little extra C# code to help with it feeling more natural.
%typemap(cscode) _Message %{
  public virtual HeaderMessagePart Header {
    get { return GetHeader(); }
  }

  public virtual BodyMessagePart Body {
    get { return GetBody(); }
  }
%}

}

%include "cpp/src/messaging/basic_binary_message.hpp"
%include "cpp/src/messaging/binary_message.h"
