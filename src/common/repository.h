#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <git2.h>

git_repository* get_git_repository();

const char *get_git_prefix();
//returns the prefix for the current working directory

void free_repository();

#endif
