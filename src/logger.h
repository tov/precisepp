#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/* from http://stackoverflow.com/questions/6168107/how-to-implement-a-good-debug-logging-feature-in-a-project */

namespace gc
{
namespace logging
{

enum class log_level_t {
    error, warning, info, debug, debug1, debug2, debug3, debug4
};

constexpr bool operator>(log_level_t a, log_level_t b)
{
    return int(a) > int(b);
}

const char* to_string(log_level_t);

unsigned int to_int(log_level_t);

std::ostream& operator<<(std::ostream& o, log_level_t level);

class Log_it
{
    std::ostringstream buffer_;

public:
    Log_it(log_level_t level = log_level_t::error) {
        buffer_ << std::setw(7) << level << ':';
        buffer_ <<
            std::string(level > log_level_t::debug
                            ? 4u * (size_t(level) - size_t(log_level_t::debug))
                            : 1u,
                        ' ');
    }

    template <typename T>
    Log_it& operator<<(T const & value)
    {
        buffer_ << value;
        return *this;
    }

    ~Log_it()
    {
        buffer_ << '\n';
        // This is atomic according to the POSIX standard
        // http://www.gnu.org/s/libc/manual/html_node/Streams-and-Threads.html
        std::cerr << buffer_.str();
    }
};

constexpr log_level_t log_level = log_level_t::debug3;

} // namespace logging
} // namespace gc

#define log(level) \
    if (int(::gc::logging::log_level_t::level) > int(::gc::logging::log_level)) { } \
    else ::gc::logging::Log_it(::gc::logging::log_level_t::level)
