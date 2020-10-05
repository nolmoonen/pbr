#ifndef UTIL_NM_LOG_HPP
#define UTIL_NM_LOG_HPP

#include <cstdarg>
#include <cstdio>

enum log_level_e {
    LOG_TRACE = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

class nm_log {
private:
    static const char *const LEVEL_NAMES[];

    static log_level_e m_level;
public:
    static void set_log_level(log_level_e t_level);

    static void log(log_level_e t_level, const char *t_format, ...);
};

#endif //UTIL_NM_LOG_HPP
