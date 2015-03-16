#include "errors.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <git2.h>

#include "git-compat-util.h"

void vreportf(const char *prefix, const char *err, va_list params)
{
	char msg[4096];
	vsnprintf(msg, sizeof(msg), err, params);
	fprintf(stderr, "%s%s\n", prefix, msg);
}

static void die_builtin(const char *err, va_list params)
{
	vreportf("fatal: ", err, params);
}

static void libgit_error_builtin()
{
	if (!giterr_last())
	{
		fprintf(stderr, "libgit error: No detailed error message!\n");
	} else
	{
		char *message = giterr_last()->message;
		fprintf(stderr, "libgit error (%d): %s\n", giterr_last()->klass, message);
	}
}

static void (*die_routine)(const char *err, va_list params) = die_builtin;
static void (*libgit_error_routine)() = libgit_error_builtin;

void set_die_routine(NORETURN_PTR void (*routine)(const char *err, va_list params))
{
		die_routine = routine;
}

void die(const char *err, ...)
{
	va_list params;

	va_start(params, err);
	die_routine(err, params);
	va_end(params);
	do_exit(128);
}

void die_errno(const char *fmt, ...)
{
	va_list params;
	char fmt_with_err[1024];
	char str_error[256], *err;
	unsigned int i, j;

	err = strerror(errno);
	for (i = j = 0; err[i] && j < sizeof(str_error) - 1; ) {
		if ((str_error[j++] = err[i++]) != '%')
			continue;
		if (j < sizeof(str_error) - 1) {
			str_error[j++] = '%';
		} else {
			/* No room to double the '%', so we overwrite it with
			 * '\0' below */
			j--;
			break;
		}
	}
	str_error[j] = 0;
	snprintf(fmt_with_err, sizeof(fmt_with_err), "%s: %s", fmt, str_error);

	va_start(params, fmt);
	die_routine(fmt_with_err, params);
	va_end(params);
	do_exit(128);
}

void libgit_error()
{
	libgit_error_routine();
}

void do_exit(int exit_code)
{
	exit(exit_code);
}
