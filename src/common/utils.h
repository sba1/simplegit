#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

typedef void (*try_to_free_t)(size_t);
try_to_free_t set_try_to_free_routine(try_to_free_t routine);

char *xstrdup(const char *str);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
ssize_t xread(int fd, void *buf, size_t len);
ssize_t xwrite(int fd, const void *buf, size_t len);
ssize_t read_in_full(int fd, void *buf, size_t count);
ssize_t write_in_full(int fd, const void *buf, size_t count);
int xdup(int fd);
FILE *xfdopen(int fd, const char *mode);

#endif
