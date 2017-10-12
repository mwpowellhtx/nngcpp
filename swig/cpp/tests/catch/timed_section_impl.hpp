#ifndef NNGCPP_TIMED_SECTION_IMPL_HPP
#define NNGCPP_TIMED_SECTION_IMPL_HPP

#include <catch.hpp>

#include <chrono>
#include <functional>

namespace nng {
    
    template<class Duration_>
    struct timed_section_impl {
        private:
            timed_section_impl() {}

            typedef std::chrono::high_resolution_clock high_resolution_clock;

            template <class Clock_ = high_resolution_clock>
            static decltype(Clock_::now()) now() { return Clock_::now(); }

        public:

            typedef std::function<void(void)> section_callback;

            static void run(const Duration_ timeout, const section_callback& callback) {

                using namespace std::chrono;

                const auto before = now();
                REQUIRE(before.time_since_epoch().count() > 0);

                const auto expected_min_timeout = before + timeout;
                const auto expected_max_timeout = before + timeout * 2;

                callback();

                const auto after = now();

                // TODO: TBD: ugh... timing is such a difficult thing to gauge accurately...
                // TODO: TBD: Catch does not support std::chrono::duration (yet...), so in the meantime, this will at least yield numbers that we can review in the logs.

                REQUIRE(after.time_since_epoch().count() >= expected_min_timeout.time_since_epoch().count());
                REQUIRE(after.time_since_epoch().count() < expected_max_timeout.time_since_epoch().count());

                //REQUIRE(after.time_since_epoch().count() >= (before + timeout).time_since_epoch().count());
                //REQUIRE(after.time_since_epoch().count() < (before + timeout * 2).time_since_epoch().count());
            }
    };

    typedef timed_section_impl<std::chrono::microseconds> microsecond_timed_section_impl;
    typedef timed_section_impl<std::chrono::milliseconds> millisecond_timed_section_impl;
    typedef timed_section_impl<std::chrono::seconds> second_timed_section_impl;
}

#define RUN_TIMED_SECTION_MILLISECONDS(timeout, callback) nng::millisecond_timed_section_impl::run(timeout, callback)


#endif // NNGCPP_TIMED_SECTION_IMPL_HPP
