#ifndef GIT_COMMIT_H
#define GIT_COMMIT_H

#include <git2.h>

int cmd_commit(git_repository *repo, int argc, char **argv);

#endif

