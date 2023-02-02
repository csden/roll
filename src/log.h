#ifndef LOG_H_
#define LOG_H

#include <stdio.h>

#define LOG_PREFIX_ERROR "err: "

#define __logf(dest, placeholder, ...)                                         \
  fprintf(dest, placeholder "\n", ##__VA_ARGS__)

#define __log(dest, message) fputs(message "\n", dest)

#define logf(placeholder, ...) __logf(stdout, placeholder, ##__VA_ARGS__)
#define errorf(placeholder, ...)                                               \
  __logf(stderr, LOG_PREFIX_ERROR placeholder, ##__VA_ARGS__)

#define log(message) __log(stdout, message)
#define error(message) __log(stderr, LOG_PREFIX_ERROR message)

#endif
