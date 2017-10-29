#include "reader_writer.h"

namespace nng {

    _OptionReaderWriter::_OptionReaderWriter()
        : _OptionReader()
        , _OptionWriter() {
    }

    _OptionReaderWriter::~_OptionReaderWriter() {
    }

    void _OptionReaderWriter::get(const std::string& name, void* valp, size_type& szp) {
        reader_type::get(name, valp, szp);
    }

    std::string _OptionReaderWriter::GetText(const std::string& name) {
        return reader_type::GetText(name);
    }

    std::string _OptionReaderWriter::GetText(const std::string& name, size_type* const szp) {
        return reader_type::GetText(name, szp);
    }

    void _OptionReaderWriter::get(const std::string& name, _SockAddr& val) {
        reader_type::get(name, val);
    }

    int32_t _OptionReaderWriter::GetInt32(const std::string& name) {
        return reader_type::GetInt32(name);
    }

    size_type _OptionReaderWriter::GetSize(const std::string& name) {
        return reader_type::GetSize(name);
    }

    duration_type _OptionReaderWriter::GetDuration(const std::string& name) {
        return reader_type::GetDuration(name);
    }

    duration_rep_type _OptionReaderWriter::GetMilliseconds(const std::string& name) {
        return reader_type::GetMilliseconds(name);
    }

    void _OptionReaderWriter::set(const std::string& name, const void* valp, size_type sz) {
        writer_type::set(name, valp, sz);
    }

    void _OptionReaderWriter::SetString(const std::string& name, const std::string& s) {
        writer_type::SetString(name, s);
    }

    void _OptionReaderWriter::SetInt32(const std::string& name, int32_t val) {
        writer_type::SetInt32(name, val);
    }

    void _OptionReaderWriter::SetSize(const std::string& name, size_type val) {
        writer_type::SetSize(name, val);
    }

    void _OptionReaderWriter::SetDuration(const std::string& name, const duration_type& val) {
        writer_type::SetDuration(name, val);
    }

    void _OptionReaderWriter::SetMilliseconds(const std::string& name, duration_rep_type val) {
        writer_type::SetMilliseconds(name, val);
    }
}
