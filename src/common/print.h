#ifndef SGIT_PRINT_H
#define SGIT_PRINT_H

#include <git2.h>

void print_commit(git_commit *wcommit, int with_tree);

#endif
