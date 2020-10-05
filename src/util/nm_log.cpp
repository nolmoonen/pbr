#include "nm_log.hpp"

const char *const nm_log::LEVEL_NAMES[] = {
        "TRACE", "INFO ", "WARN ", "ERROR"
};

log_level_e nm_log::m_level = LOG_TRACE;

void nm_log::set_log_level(log_level_e t_level)
{
    m_level = t_level;
}

void nm_log::log(log_level_e t_level, const char *t_format, ...)
{
    if (t_level >= m_level) {
        fprintf(stdout, "%s ", LEVEL_NAMES[t_level]);
        va_list argptr;
        va_start(argptr, t_format);
        vfprintf(stdout, t_format, argptr);
        va_end(argptr);
    }
}
