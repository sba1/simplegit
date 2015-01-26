#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

typedef void (*try_to_free_t)(size_t);
try_to_free_t set_try_to_free_routine(try_to_free_t routine);

char *xstrdup(const char *str);
void *xmalloc(size_t size);
void *xmallocz(size_t size);
void *xmemdupz(const void *data, size_t len);
char *xstrndup(const char *str, size_t len);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
ssize_t xread(int fd, void *buf, size_t len);

/**
 * Join two paths together. Takes care of properly fixing the
 * middle slashes and everything
 *
 * The paths are joined together into buffer_out; this is expected
 * to be an user allocated buffer of `GIT_PATH_MAX` size 
 */
void git2__joinpath_n(char *buffer_out, int npath, ...);

void git2__joinpath(char *buffer_out, const char *path_a, const char *path_b);

#endif
