/**
 * @file
 */
#ifndef BUILTIN_STASH_H_
#define BUILTIN_STASH_H_

#include <git2.h>

int cmd_stash(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_STASH_H_ */
