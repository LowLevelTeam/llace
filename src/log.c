#include "llace/log.h"

static const char *level_strings[] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

void llace_log(int level, const char *file, int line, const char *function, const char *fmt, ...) {
  char timebuf[16];
  time_t t = time(NULL);
  timebuf[strftime(timebuf, sizeof(timebuf), "%H:%M:%S", localtime(&t))] = '\0';

  fprintf(
    stdout, "%s %s%-5s\x1b[0m \x1b[90m%s:%d: (%s)\x1b[0m ",
    timebuf, level_colors[level], level_strings[level],
    file, line, function
  );

  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);

  fprintf(stdout, "\n");
  fflush(stdout);
}