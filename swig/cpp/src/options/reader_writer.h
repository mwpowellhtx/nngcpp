#ifndef NNGCPP_OPTIONS_READER_WRITER_H
#define NNGCPP_OPTIONS_READER_WRITER_H

#include "reader.h"
#include "writer.h"

namespace nng {

#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
    template<class Options_> struct IHaveOptions;
#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

    class _OptionReaderWriter : public _OptionReader, public _OptionWriter {
    private:

        typedef _OptionReader reader_type;
        typedef _OptionWriter writer_type;

    protected:

        template<class Options_> friend struct IHaveOptions;

        _OptionReaderWriter();

    public:

        virtual ~_OptionReaderWriter();

        virtual void get(const std::string& name, void* valp, size_type& szp) override;

        virtual std::string GetText(const std::string& name) override;
        virtual std::string GetText(const std::string& name, size_type* const szp) override;

        virtual void get(const std::string& name, address& val) override;

        virtual int32_t GetInt32(const std::string& name) override;
        virtual size_type GetSize(const std::string& name) override;

        virtual duration_type GetDuration(const std::string& name) override;
        virtual duration_rep_type GetMilliseconds(const std::string& name) override;

        virtual void set(const std::string& name, const void* valp, size_type sz) override;
        virtual void SetString(const std::string& name, const std::string& s) override;

        virtual void SetInt32(const std::string& name, int32_t val) override;
        virtual void SetSize(const std::string& name, size_type val) override;

        virtual void SetDuration(const std::string& name, const duration_type& val) override;
        virtual void SetMilliseconds(const std::string& name, duration_rep_type val) override;
    };

    typedef _OptionReaderWriter options_reader_writer;
}

#endif // NNGCPP_OPTIONS_READER_WRITER_H
