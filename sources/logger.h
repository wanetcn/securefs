#pragma once
#include "exceptions.h"
#include "streams.h"

#include <memory>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string>

namespace securefs
{
enum LoggingLevel
{
    kLogTrace = 0,
    kLogDebug = 1,
    kLogInfo = 2,
    kLogWarning = 3,
    kLogError = 4
};

inline const char* stringify(LoggingLevel lvl)
{
    switch (lvl)
    {
    case kLogTrace:
        return "Trace";
    case kLogDebug:
        return "Debug";
    case kLogInfo:
        return "Info";
    case kLogWarning:
        return "Warning";
    case kLogError:
        return "Error";
    }
    return "UNKNOWN";
}

class Logger
{
    DISABLE_COPY_MOVE(Logger)

private:
    LoggingLevel m_level;
    FILE* m_fp;
    bool m_close_on_exit;

    explicit Logger(FILE* fp, bool close_on_exit);

public:
    static Logger* create_stderr_logger();
    static Logger* create_file_logger(const std::string& path);

    void vlog(LoggingLevel level, const char* format, va_list args) noexcept;
    void log(LoggingLevel level, const char* format, ...) noexcept
#ifndef WIN32
        __attribute__((format(printf, 3, 4)))
#endif
        ;

    LoggingLevel get_level() const noexcept { return m_level; }
    void set_level(LoggingLevel lvl) noexcept { m_level = lvl; }

    ~Logger();
};

extern Logger* global_logger;

#define GENERIC_LOG(log_level, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        if (global_logger && global_logger->get_level() <= log_level)                              \
        {                                                                                          \
            global_logger->log(log_level, __VA_ARGS__);                                            \
        }                                                                                          \
    } while (0)
#define TRACE_LOG(...) GENERIC_LOG(kLogTrace, __VA_ARGS__)
#define DEBUG_LOG(...) GENERIC_LOG(kLogDebug, __VA_ARGS__)
#define INFO_LOG(...) GENERIC_LOG(kLogInfo, __VA_ARGS__)
#define WARN_LOG(...) GENERIC_LOG(kLogWarning, __VA_ARGS__)
#define ERROR_LOG(...) GENERIC_LOG(kLogError, __VA_ARGS__)
}
