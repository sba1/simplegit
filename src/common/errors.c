#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <git2.h>
#include "git.h"
#include "errors.h"

void vreportf(const char *prefix, const char *err, va_list params)
{
	char msg[4096];
	vsnprintf(msg, sizeof(msg), err, params);
	fprintf(stderr, "%s%s\n", prefix, msg);
}

static void usage_builtin(const char *err, va_list params)
{
	vreportf("usage: ", err, params);
}

static void die_builtin(const char *err, va_list params)
{
	vreportf("fatal: ", err, params);
}

static void error_builtin(const char *err, va_list params)
{
	vreportf("error: ", err, params);
}

static void warn_builtin(const char *warn, va_list params)
{
	vreportf("warning: ", warn, params);
}

static void libgit_error_builtin()
{
	fprintf(stderr, "libgit error: %s\n", git_lasterror()/*git_strerror(error_code)*/);
}

static void (*usage_routine)(const char *err, va_list params) = usage_builtin;
static void (*die_routine)(const char *err, va_list params) = die_builtin;
static void (*error_routine)(const char *err, va_list params) = error_builtin;
static void (*warn_routine)(const char *err, va_list params) = warn_builtin;
static void (*libgit_error_routine)() = libgit_error_builtin;

void NORETURN usagef(const char *err, ...)
{
	va_list params;

	va_start(params, err);
	usage_routine(err, params);
	va_end(params);
	do_exit(129);
}

void NORETURN usage(const char *err)
{
	usagef("%s", err);
}

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

int error(const char *err, ...)
{
	va_list params;

	va_start(params, err);
	error_routine(err, params);
	va_end(params);
	exit(-1);
}

void warning(const char *warn, ...)
{
	va_list params;

	va_start(params, warn);
	warn_routine(warn, params);
	va_end(params);
}

void libgit_error()
{
	libgit_error_routine();
	do_exit(128);
}

void do_exit(int exit_code)
{
	free_global_resources();
	exit(exit_code);
}
