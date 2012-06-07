#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <stdarg.h>
#define GIT_OLD_ERRORS
#include <git2/errors.h>

#include "git-compat-util.h"

#define git_tree_close(x) git_object_free(x)
#define git_commit_close(x) git_object_free(x)

void vreportf(const char *prefix, const char *err, va_list params);
NORETURN void usage(const char *err);
NORETURN void usagef(const char *err, ...) __attribute__((format (printf, 1, 2)));
NORETURN void die(const char *err, ...) __attribute__((format (printf, 1, 2)));
NORETURN void die_errno(const char *err, ...) __attribute__((format (printf, 1, 2)));
int error(const char *err, ...) __attribute__((format (printf, 1, 2)));
void warning(const char *err, ...) __attribute__((format (printf, 1, 2)));
void libgit_error();

void set_die_routine(NORETURN_PTR void (*routine)(const char *err, va_list params));
void NORETURN do_exit();

#endif
