#ifndef GIT_WRITE_TREE_H
#define GIT_WRITE_TREE_H

#include <git2.h>

int cmd_write_tree(git_repository *repo, int argc, char **argv);

#endif
