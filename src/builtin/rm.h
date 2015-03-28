/**
 * @file
 */
#ifndef BUILTIN_RM_H_
#define BUILTIN_RM_H_

#include <git2.h>

int cmd_rm(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_RM_H_ */
