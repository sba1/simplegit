/**
 * @file
 */
#ifndef BUILTIN_REMOTE_H_
#define BUILTIN_REMOTE_H_

#include <git2.h>

int cmd_remote(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_REMOTE_H_ */
