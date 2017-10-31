#ifndef NNGCPP_ENDPOINT_H
#define NNGCPP_ENDPOINT_H

#include "enums.h"

#include "IHaveOne.hpp"
#include "ICanClose.hpp"

#include "../options/reader_writer.h"
#include "../options/IHaveOptions.hpp"

namespace nng {

    // TODO: TBD: carries along with it information about ::nng_pipe ... just a matter of how to present it to the C++ (or SWIG) community...
    class _EndPoint
        : public IHaveOne
        , public ICanClose
        , public IHaveOptions<_OptionReaderWriter> {

    protected:

        typedef _EndPoint ep_type;

        typedef std::function<int(int)> start_func;
        typedef std::function<int()> close_func;

        const start_func __start;
        const close_func __close;

        void configure_endpoint(
            const start_func& start
            , const close_func& close);

        _EndPoint();

    public:

        virtual ~_EndPoint();

        virtual void Start() = 0;

        virtual void Start(SocketFlag flags) = 0;
    };
}

#endif // NNGCPP_ENDPOINT_H
