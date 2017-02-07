/**
 * @file
 */
#ifndef BUILTIN_SUBMODULE_H_
#define BUILTIN_SUBMODULE_H_

#include <git2.h>

int cmd_submodule(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_RESET_H_ */
