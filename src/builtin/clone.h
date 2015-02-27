#ifndef GIT_CLONE_H
#define GIT_CLONE_H

#include <git2.h>

int do_clone(git_repository *repo, int argc, char **argv);

#endif

