#ifndef CATCH_MATCHERS_REGEX_HPP
#define CATCH_MATCHERS_REGEX_HPP

#include <catch.hpp>

#include <string>
#include <regex>

namespace Catch {
    namespace Matchers {
        namespace RegEx {

            struct RegExMatcher : MatcherBase<std::string> {
            private:

                const std::string _pattern;
                const std::regex _regex;

            public:

                RegExMatcher(std::string pattern)
                    : MatcherBase()
                    , _pattern(pattern)
                    , _regex(pattern) {
                }

                virtual bool match(const std::string& subject) const override {
                    std::smatch m;
                    const auto& s = subject;
                    std::regex_match(s.cbegin(), s.cend(), m, _regex);
                    return m.ready() && !m.empty();
                }

                virtual std::string describe() const {
                    std::ostringstream os;
                    os << "expected regular expression matching pattern ( " << _pattern << " )";
                    return os.str();
                }
            };
        }

        RegEx::RegExMatcher MatchesRegEx(const std::string& pattern) {
            return RegEx::RegExMatcher(pattern);
        }
    }
}

#endif // CATCH_MATCHERS_REGEX_HPP
