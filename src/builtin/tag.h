/**
 * @file
 */
#ifndef BUILTIN_TAG_H_
#define BUILTIN_TAG_H_

#include <git2.h>

int cmd_tag(git_repository *repo, int argc, char **argv);

#endif /* BUILTIN_TAG_H_ */
