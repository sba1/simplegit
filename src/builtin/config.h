#ifndef GIT_CONFIG_H
#define GIT_CONFIG_H

#include <git2.h>

int cmd_config(git_repository *repo, int argc, char **argv);

#endif
