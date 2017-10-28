#ifndef NNGCPP_OPTIONS_READER_WRITER_H
#define NNGCPP_OPTIONS_READER_WRITER_H

#include "reader.h"
#include "writer.h"

#include <string>
#include <functional>

namespace nng {

    class options_reader_writer : public options_reader, public options_writer {
    public:

        options_reader_writer();

        virtual ~options_reader_writer();
    };
}

#endif // NNGCPP_OPTIONS_READER_WRITER_H
