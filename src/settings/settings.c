#include <stdarg.h>
#include <stdio.h>

#include "c_minilib_sip_codec.h"

#define CMSC_LOG_BUF_SIZE 512

static struct cmsc_Settings cmsc_settings = {0};

void cmsc_configure(const struct cmsc_Settings settings) {
  cmsc_settings = settings;
}

void cmsc_log(enum cmsc_LogLevels log_lvl, const char *fmt, ...) {
  if (!cmsc_settings.log_func || !fmt) {
    return;
  }

  char buf[CMSC_LOG_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  cmsc_settings.log_func(log_lvl, buf);
}
