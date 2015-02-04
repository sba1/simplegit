#ifndef GIT_CHERRY_PICK_H
#define GIT_CHERRY_PICK_H

#include <git2.h>

int cmd_cherry_pick(git_repository *repo, int argc, char **argv);

#endif
