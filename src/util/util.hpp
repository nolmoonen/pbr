#ifndef PBR_UTIL_HPP
#define PBR_UTIL_HPP

#include <vector>
#include <cstdlib>

#include "nm_log.hpp"

namespace Util {
    int read_file(char **buffer, size_t *size, const char *file_name);
}

#endif //PBR_UTIL_HPP
