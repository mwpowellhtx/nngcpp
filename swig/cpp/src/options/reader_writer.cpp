#include "reader_writer.h"
#include "../transport/address.h"
#include "../core/invocation.hpp"
#include "../algorithms/string_algo.hpp"

namespace nng {

    _OptionReaderWriter::_OptionReaderWriter()
        : _BasicOptionReader()
        , _BasicOptionWriter() {
    }

    _OptionReaderWriter::~_OptionReaderWriter() {
    }

    void _OptionReaderWriter::get(const std::string& name, void* valp, size_type& sz) {
        invocation::with_default_error_handling(_getopt, name.c_str(), valp, &sz);
    }

    std::string _OptionReaderWriter::GetText(const std::string& name) {
        // TODO: TBD: for lack of a better definition for the moment...
        size_type sz = _MAX_PATH;
        return GetText(name, sz);
    }

    std::string _OptionReaderWriter::GetText(const std::string& name, size_type& sz) {
        std::string s;
        s.resize(sz);
        invocation::with_default_error_handling(_getopt, name.c_str(), &s[0], &sz);
        /* So we do use the string trimming algorithms after all...
        Which the in-place is sufficient, no need to use the copying version. */
        return trx::trimcp(s);
    }

    int32_t _OptionReaderWriter::GetInt32(const std::string& name) {
        int val;
        invocation::with_default_error_handling(_getopt_int, name.c_str(), &val);
        return val;
    }

    size_type _OptionReaderWriter::GetSize(const std::string& name) {
        size_type result;
        invocation::with_default_error_handling(_getopt_sz, name.c_str(), &result);
        return result;
    }

    duration_type _OptionReaderWriter::GetDuration(const std::string& name) {
        auto x = GetMilliseconds(name);
        return duration_type(x);
    }

    duration_rep_type _OptionReaderWriter::GetMilliseconds(const std::string& name) {
        duration_rep_type result;
        invocation::with_default_error_handling(_getopt_duration, name.c_str(), &result);
        return result;
    }

    _SockAddr _OptionReaderWriter::GetSocketAddress(const std::string& name) {
        _SockAddr result;
        auto sz = result.GetSize();
        invocation::with_default_error_handling(_getopt, name.c_str(), result.get(), &sz);
        return result;
    }

    void _OptionReaderWriter::set(const std::string& name, const void* valp, size_type sz) {
        invocation::with_default_error_handling(_setopt, name.c_str(), valp, sz);
    }

    void _OptionReaderWriter::SetString(const std::string& name, const std::string& s) {
        invocation::with_default_error_handling(_setopt, name.c_str(), s.c_str(), s.length());
    }

    void _OptionReaderWriter::SetInt32(const std::string& name, int32_t val) {
        invocation::with_default_error_handling(_setopt_int, name.c_str(), val);
    }

    void _OptionReaderWriter::SetSize(const std::string& name, size_type val) {
        invocation::with_default_error_handling(_setopt_sz, name.c_str(), val);
    }

    void _OptionReaderWriter::SetDuration(const std::string& name, const duration_type& val) {
        SetMilliseconds(name, val.count());
    }

    void _OptionReaderWriter::SetMilliseconds(const std::string& name, duration_rep_type val) {
        invocation::with_default_error_handling(_setopt_duration, name.c_str(), val);
    }
}
