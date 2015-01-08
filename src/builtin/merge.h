/**
 * @file
 */
#ifndef BUILTIN_MERGE_H_
#define BUILTIN_MERGE_H_

#include <git2.h>

int cmd_merge(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_MERGE_H_ */
