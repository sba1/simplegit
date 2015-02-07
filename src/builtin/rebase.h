/**
 * @file
 */
#ifndef BUILTIN_REBASE_H_
#define BUILTIN_REBASE_H_

#include <git2.h>

int cmd_rebase(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_REBASE_H_ */
