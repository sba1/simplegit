#ifndef GIT_PULL_H
#define GIT_PULL_H

#include <git2.h>

int cmd_pull(git_repository *repo, int argc, char **argv);

#endif
