#ifndef GIT_CHECKOUT_H
#define GIT_CHECKOUT_H

#include <git2.h>

int cmd_checkout(git_repository *repo, int argc, char **argv);

#endif
