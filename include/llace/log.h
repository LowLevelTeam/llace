#ifndef LLACE_LOG_H
#define LLACE_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#ifndef __FUNCTION__
  #ifdef __func__
    #define __FUNCTION__ __func__
  #else
    #define __FUNCTION__ "{No Function Information}"
  #endif
#endif

#define LLACE_LOG_TRACE(...) llace_log(LLACE_LOG_TRACE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LLACE_LOG_DEBUG(...) llace_log(LLACE_LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LLACE_LOG_INFO(...) llace_log(LLACE_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LLACE_LOG_WARN(...) llace_log(LLACE_LOG_WARN, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LLACE_LOG_ERROR(...) llace_log(LLACE_LOG_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LLACE_LOG_FATAL(...) llace_log(LLACE_LOG_FATAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); abort()

// TODO macro for unimplemented functionality
// Helps with incrementel development
#define LLACE_TODO(msg) LLACE_LOG_FATAL("TODO: '%s' not implemented\n", msg); 
#define LLACE_TODOV(fmt, ...) LLACE_LOG_FATAL("TODO: '"fmt"' not implemented\n", __VA_ARGS__);

enum { LLACE_LOG_TRACE, LLACE_LOG_DEBUG, LLACE_LOG_INFO, LLACE_LOG_WARN, LLACE_LOG_ERROR, LLACE_LOG_FATAL };
void llace_log(int level, const char *file, int line, const char *function, const char *fmt, ...);

#endif // LLACE_LOG_H