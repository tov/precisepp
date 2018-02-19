#include "logger.h"

namespace gc
{

namespace logging
{

const char* to_string(log_level_t level)
{
    switch (level) {
        case log_level_t::error:
            return "error";
        case log_level_t::warning:
            return "warning";
        case log_level_t::info:
            return "info";
        case log_level_t::debug:
            return "debug";
        case log_level_t::debug1:
            return "debug(1)";
        case log_level_t::debug2:
            return "debug(2)";
        case log_level_t::debug3:
            return "debug(3)";
        case log_level_t::debug4:
            return "debug(4)";
    }
}

std::ostream& operator<<(std::ostream& o, log_level_t level)
{
    return o << to_string(level);
}

} // namespace logging
} // namespace gc

