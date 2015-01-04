#ifndef GIT_DIFF_H
#define GIT_DIFF_H

#include <git2.h>

int cmd_diff(git_repository *repo, int argc, char **argv);

#endif
