#ifndef GIT_CHECKOUT_INDEX_H
#define GIT_CHECKOUT_INDEX_H

#include <git2.h>

int cmd_checkout_index(git_repository *repo, int argc, char **argv);

#endif
