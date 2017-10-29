#include "reader.h"
#include "../transport/address.h"
#include "../core/invocation.hpp"
#include "../algorithms/string_algo.hpp"

namespace nng {

    _OptionReader::_OptionReader()
        : _getopt()
        , _getopt_int()
        , _getopt_sz()
        , _getopt_duration() {
    }

    void _OptionReader::set_getters(const getopt_func& getopt
        , const getopt_int_func& getopt_int
        , const getopt_sz_func& getopt_sz
        , const getopt_duration_func& getopt_duration) {

        // Ditto casting constness away...
        const_cast<getopt_func&>(_getopt) = getopt;
        const_cast<getopt_int_func&>(_getopt_int) = getopt_int;
        const_cast<getopt_sz_func&>(_getopt_sz) = getopt_sz;
        const_cast<getopt_duration_func&>(_getopt_duration) = getopt_duration;
    }

    _OptionReader::~_OptionReader() {
    }

    void _OptionReader::get(const std::string& name, void* valp, size_type& sz) {
        invocation::with_default_error_handling(_getopt, name.c_str(), valp, &sz);
    }

    std::string _OptionReader::GetText(const std::string& name) {
        // TODO: TBD: for lack of a better definition for the moment...
        size_type sz = _MAX_PATH;
        return GetText(name, &sz);
    }

    std::string _OptionReader::GetText(const std::string& name, size_type* const szp) {
        std::string s;
        s.resize(*szp);
        invocation::with_default_error_handling(_getopt, name.c_str(), &s[0], szp);
        /* So we do use the string trimming algorithms after all...
        Which the in-place is sufficient, no need to use the copying version. */
        return trx::trimcp(s);
    }

    void _OptionReader::get(const std::string& name, _SockAddr& val) {
        auto sz = val.GetSize();
        invocation::with_default_error_handling(_getopt, name.c_str(), val.get(), &sz);
    }

    int _OptionReader::GetInt32(const std::string& name) {
        int val;
        invocation::with_default_error_handling(_getopt_int, name.c_str(), &val);
        return val;
    }

    size_type _OptionReader::GetSize(const std::string& name) {
        size_type result;
        invocation::with_default_error_handling(_getopt_sz, name.c_str(), &result);
        return result;
    }

    duration_type _OptionReader::GetDuration(const std::string& name) {
        auto x = GetMilliseconds(name);
        return duration_type(x);
    }

    duration_rep_type _OptionReader::GetMilliseconds(const std::string& name) {
        duration_rep_type result;
        invocation::with_default_error_handling(_getopt_duration, name.c_str(), &result);
        return result;
    }
}
