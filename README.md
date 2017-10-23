## Welcome to C++ NNG

This library is at its core a C++1y compliant wrapper around the [next generation nanomsg](http://github.com/nanomsg/nng) SP protocol.

As NNG is a work in progress, so too must this repository necessarily be considered a work in progress.

### Supported platforms

At the time of this writing, I've focused primarily on the Visual Studio 2015 C++ compiler, latest updates, running under 64-bit Windows 7 Professional SP1.

That I know of, I am not doing anything that exotic or cutting edge for C++14. Most of the features, using [functional](http://en.cppreference.com/w/cpp/header/functional), [std::bind](http://en.cppreference.com/w/cpp/utility/functional/bind), [std::function](http://en.cppreference.com/w/cpp/utility/functional/function), as well as strategic use of both concretely bound parameters and [placeholders](http://en.cppreference.com/w/cpp/utility/functional/placeholders), have been around since C++11 days, that I know of. There may be one or two features, such as variadics or template specialization, that push the limits, but not many others to my knowledge.

### Build instructions

At the present time I do not have CI/CD supported, however, this remains one of my goals.

You will at least require [*CMake 3.9*](http://cmake.org/download/) in order to build and test yourself. Specifically, I am running the latest *CMake 3.9.2*, and 3.9.4 is stable and available for download at the time of this writing, with *3.10* just around the corner (currently RC). Point CMake to your repository directory and indicate the build directory, such as *path/to/your/repo/build*. After that, simply configure and generate to produce your build environment.

At the present time I know that this project's CMake support is almost exclusively geared around Visual Studio, 2015 specifically. If you would like to venture into other platforms or compilers, feel free to submit your pull request for consideration.

### Current status

The library is composed of several key namespaces primarily. The first is ``nng`` for the core ``socket`` support, however, you ought never to be using ``socket`` directly, except perhaps as an abstract base class. The sockets you are concerned about live in ``nng::protocol``, and are dubbed *latest_xyz_socket*. Such as ``latest_pair_socket`` for the latest version ``pair_socket``.

#### Messaging

*Messaging* support is fairly robust at the time of this writing and are exposed via the ``nng::messaging`` namespace. I've made a serious effort to make sure that this works and is robust, with a battery of unit tests to demonstrate as much. This is a key feature of the library and puts the *"msg"* in *"nanomsg"*, in my opinion. Chiefly you should be concerned with the ``binary_message``, which I know is sort of an oxymoron in that all messages are binary, technically speaking. However, there is rich support around messages, with operators to support working with them that are highly extensible using specialized templates.

#### API differences

For the most part, I've made an effort to remain consistent with the underlying C API as far as naming conventions are concerned. However, there are a few differences in the way that API is supported. Messaging ``body`` supports the full range of possible API, whereas ``header`` esposure is somewhat limited, and then this was only due to exposure during unit testing. Of course, this is a work in progress and may change on a moment's notice.

Okay, now for those differences. The ``append`` operation still appends in NNG parlance. However, I've dubbed *insert* instead as ``prepend``, which is commented as such anyway. So let's just call it what it is. Additionally, I just got confused every time I try to parse what *trim* and *chop* mean to the message, so I dubbed them simply ``ltrim`` and ``rtrim``, respectively.

There is broad support for ``uint32_t`` in both ``header`` and ``body`` messaging parts. This literally *appends* and *prepends* the actual value itself *at the byte level*. *Trimming left* or *right* behaves by returning the *actual value*.

There is limited support for ``std::vector<uint8_t>`` via the ``body`` messaging part only. The API itself is also somewhat limited, just in my opinion. You can *append* and *prepend* the vector itself. However, *trim* only permits you to specify the ``size_t``, ``std::size_t``, ``std::vector<uint8_t>::size_type``, or ``nng::size_type`` (all definitions of the same underlying type), which effectively *trims left* or *right* that number of bytes. This is performed blindly without returning the bytes that were trimmed.

You may verify any of these features afterwards by using the ``get_size()`` and ``get()`` methods, which return ``nng::size_type`` and ``std::vector<uint8_t>``, respectively.

#### Options

There are several classes that support an ``options()`` access method, which permits *getting* or *setting* options of one form or another, depending on the root class. Classes that support the ``options_reader`` API include ``socket``, ``listener``, ``dialer``, and ``message_pipe``. Except for the ``message_pipe``, each of these classes also support the ``options_writer``.

At the time of this writing, options are *indexed* according to ``std::string`` name, defined by constants in the ``nng::option_names`` class. This is also fairly freeform in nature, allowing you to specify any of the names for any of the API.

I will not go into excruciating detail over the API itself, but just list briefly the kinds of options you may get and set, including, ``std::string``, 32-bit integer values (``int`` or ``int32_t``), ``nng::size_type`` (or ``std::size_t``), and ``std::chrono::duration<int32_t, std::milli>`` durations, as well as durations in raw ``int32_t`` milliseconds.

Generally, however, it is up to the consumer to align the appropriate option names with the appropriate options during either get or set operations. The API will throw an unsupported exception when that combination is inappropriate.

#### Socket address

There is also support around *socket addresses*, which facilitates a rich set of ``socket address`` views for you, depending on the kind of view that is in play. This is a fairly new feature discovered only recently during unit testing. For the most part you should be engaging the API with ``std::string`` addresses, but in the future, you may also be able to utilize this feature.

#### Compatibility mode

I have also made the decision not to support the [legacy library](https//github.com/nanomsg/nanomsg). Not even in the NNG *compatibility* mode. After discussing with the NNG team, we agree that this adds no value, but if community contributors would like to see it, by all means. I will consider the pull request.

### Unit tests

Much of the wrapper functionality has been fully tested and vetted under 64-bit Windows 7 Professional SP1. There are one or two areas that are known issues which I will document a little later on.

I am leveraging [Catch v1.10](http://github.com/philsquared/Catch/releases/tag/v1.10.0) at the present time in order to facilitate unit testing. I've even written a handful of helpful extensions and features complementing the library, as well as provided feedback contributing to the Catch team's v2.0 when it finally gets released. In the meanwhile, I have found it to work fairly well but for a few glaring deficiencies.

* Lack of exception level filtering. I've provided a [crude Exception Matcher](http://github.com/mwpowellhtx/cppnngswig/blob/master/swig/cpp/tests/catch/catch_nng_exception_matcher.hpp) for this purpose, but first-class support is on the way that I know of (Catch v2.0).

* Not (I repeat: ***NOT, NOT, NOT***) thread-safe. This is perhaps the boldest weakness on the Catch palette. I'd like to do scalability testing, for example, but Catch does not support it at the present time.

* I have provided a Catch::Tags(...) variadic listing of tag names. Yes, I know, someone will say, why not just list them in a single string. And I will say, because I wanted to support vectors of them, as well as individually listed, without *[worrying][about][the][syntax]*. This is quite powerful and flexible and allows me to shim in support for things like prefix or other differences among shared unit tests.

But for these couple of areas, and lack of thread-safety is a pretty serious defect, if you ask me, but still, I cannot commend Catch highly enough. I have not used a lot of C++ test libraries. Most of my unit testing is typically using [xUnit for .NET](http://github.com/xunit/xunit), along these lines. However, for C++ unit testing, it is very strong.

### NNG dependencies and disclaimer

As NNG is not yet suitable for production, so then can this repository be considered the same. It works pretty well, and I've provided the unit tests demonstrating as much. But until the libraries are ready for broader consumption, consider yourself warned and tread with caution. [Stay tuned](http://nanomsg.org/community.html) to the repositories, web sites, and [mailing list](https://www.freelists.org/list/nanomsg) for announcements and updates.

### Known issues

* I have not had that much success with the interprocess communication (IPC) transport as exposed during the certain unit test sections. That I know of, NNG utilizes named pipes, which I think Windows 7 Professional supports, but I could be wrong. Additionally, NNG is utilizing mutexes as well as condition variables, but I have not investigated in much depth beyond that. All I know is, one of the unit tests blocks infinitely on a sleeping condition variable. Which probably means some other thread encountered an issue and failed to wake it up again, or worse, got blocked on some race-condition on the mutex. That's the extent of my contribution troubleshooting the issue to the best of my ability.

* Otherwise, transport unit tests appear to be working pretty well for *inproc* as well as for *TCP*. That said, I have not been able to test ***TCP IPv6*** that I know of, or the strategy for determining whether my platform supports it may itself be flawed. I may need to diverge from the NNG path for making this assessment, but I have not looked closely at it yet beyond the translation to C++ and to Catch.

* Also to my knowledge, NNG does not yet support ***UDP***. I think that may be in the works, but please stay tuned to the mailing list, etc, for more details. As such, the unit tests are commented out for the most part as they are exercising API internal to NNG.

* But for *Scalability* testing, which requires threading support from Catch, the other unit tests are running great. Well, to be quite frank, I suppose threading support isn't ***required*** required, but I would certainly like to have some checks being done during the thread and not just let it run freely unvetted. Let's just put it that way.

* Not an issue with the repository itself, per se, but with what appears to be CMake and/or Visual Studio support for Git commands. Neither of which seems to be too happy with pulling Git repositories to a specific hash, branch, or tag. This is critical for steps involving *Catch*, in particular, which is at least working for ``v1.10.0``, but which balks at tags such as ``V2.0.0-develop.5``. Go figure. So these steps may need doing manually, unfortunately. I may look at some workarounds if at all possible, like containing the Git commands in a batch or command file. Something like this. I just haven't cross that bridge yet.

### Future goals

* Add ***CI/CD*** support, complete with *Catch* unit test support.

* Enrich the ***cross-platform*** support. I may look at this across platforms on a Linux VM, but I will not make any promises or guarantees as such. All I can say is, community contributions are welcome. I look forward to seeing community pull requests.

* My next goals include to provide ***[SWIG](http://www.swig.org/)*** comprehension to other languages and platforms. Initially I am aiming for ***.NET and C-Sharp***, but I may also look at ***Java***. Other platforms/languages may enter into the mix as well provided the mesh well with the C++ paradigm or vice versa.

* If there is interest, I will accept pull requests for legacy compatibility mode. However, I will not be focusing on that at all at this point other than to reiterate.

Not necessarily in this order.
