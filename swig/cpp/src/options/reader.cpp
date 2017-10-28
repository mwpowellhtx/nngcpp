#include "reader.h"
#include "../core/address.h"
#include "../core/invocation.hpp"
#include "../algorithms/string_algo.hpp"

namespace nng {

    options_reader::options_reader()
        : _getopt()
        , _getopt_int()
        , _getopt_sz()
        , _getopt_duration() {
    }

    void options_reader::set_getters(const getopt_func& getopt
        , const getopt_int_func& getopt_int
        , const getopt_sz_func& getopt_sz
        , const getopt_duration_func& getopt_duration) {

        // Ditto casting constness away...
        const_cast<getopt_func&>(_getopt) = getopt;
        const_cast<getopt_int_func&>(_getopt_int) = getopt_int;
        const_cast<getopt_sz_func&>(_getopt_sz) = getopt_sz;
        const_cast<getopt_duration_func&>(_getopt_duration) = getopt_duration;
    }

    options_reader::~options_reader() {
    }

    void options_reader::get(const std::string& name, void* valp, size_type& sz) {
        invocation::with_default_error_handling(_getopt, name.c_str(), valp, &sz);
    }

    void options_reader::get(const std::string& name, std::string& val, size_type& sz) {
        val.resize(sz);
        get(name, val);
    }

    void options_reader::get(const std::string& name, std::string& val) {
        auto sz = val.size();
        invocation::with_default_error_handling(_getopt, name.c_str(), &val[0], &sz);
        /* So we do use the string trimming algorithms after all...
        Which the in-place is sufficient, no need to use the copying version. */
        trx::trim(val);
    }

    void options_reader::get(const std::string& name, address& val) {
        auto sz = val.GetSize();
        invocation::with_default_error_handling(_getopt, name.c_str(), val.get(), &sz);
    }

    void options_reader::get_int(const std::string& name, int& val) {
        invocation::with_default_error_handling(_getopt_int, name.c_str(), &val);
    }

    void options_reader::get_sz(const std::string& name, size_type& val) {
        invocation::with_default_error_handling(_getopt_sz, name.c_str(), &val);
    }

    void options_reader::get(const std::string& name, duration_type& val) {
        duration_rep_type x;
        get_milliseconds(name, x);
        val = duration_type(x);
    }

    void options_reader::get_milliseconds(const std::string& name, duration_rep_type& val) {
        invocation::with_default_error_handling(_getopt_duration, name.c_str(), &val);
    }
}
