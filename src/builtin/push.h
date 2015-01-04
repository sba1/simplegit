#ifndef GIT_PUSH_H
#define GIT_PUSH_H

#include <git2.h>

int cmd_push(git_repository *repo, int argc, char **argv);

#endif
