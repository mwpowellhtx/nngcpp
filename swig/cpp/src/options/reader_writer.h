#ifndef NNGCPP_OPTIONS_READER_WRITER_H
#define NNGCPP_OPTIONS_READER_WRITER_H

#include "reader.h"
#include "writer.h"

namespace nng {

    class _OptionReaderWriter : public _OptionReader, public _OptionWriter {
    public:

        _OptionReaderWriter();

        virtual ~_OptionReaderWriter();
    };

    typedef _OptionReaderWriter options_reader_writer;
}

#endif // NNGCPP_OPTIONS_READER_WRITER_H
