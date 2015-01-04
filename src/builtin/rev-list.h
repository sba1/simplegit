#ifndef GIT_REV_LIST_H
#define GIT_REV_LIST_H

#include <git2.h>

int cmd_rev_list(git_repository *repo, int argc, char **argv);

#endif
