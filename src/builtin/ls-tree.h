#ifndef GIT_LS_TREE_H
#define GIT_LS_TREE_H

#include <git2.h>

int cmd_ls_tree(git_repository *repo, int argc, char **argv);

#endif
