#ifndef ABSPATH_H
#define ABSPATH_H

#include "git-compat-util.h"

int is_directory(const char *path);
const char *real_path(const char *path);
const char *absolute_path(const char *path);
int is_absolute_path(const char *path);

#endif
