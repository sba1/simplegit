/**
 * @file
 */
#ifndef BUILTIN_RESET_H_
#define BUILTIN_RESET_H_

#include <git2.h>

int cmd_reset(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_RESET_H_ */
