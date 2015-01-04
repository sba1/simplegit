#ifndef GIT_LOG_H
#define GIT_LOG_H

#include <git2.h>

int cmd_log(git_repository *repo, int argc, char **argv);

#endif
