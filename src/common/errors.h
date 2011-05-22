#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <stdarg.h>

void die(const char *err, ...) __attribute__((noreturn, format (printf, 1, 2)));
void die_errno(const char *err, ...) __attribute__((noreturn, format (printf, 1, 2)));
int error(const char *err, ...) __attribute__((format (printf, 1, 2)));
void warning(const char *err, ...) __attribute__((format (printf, 1, 2)));

void set_die_routine(void (*routine)(const char *err, va_list params));

#endif
