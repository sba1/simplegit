#ifndef GIT_STATUS_H
#define GIT_STATUS_H

#include <git2.h>

int cmd_status(git_repository *repo, int argc, char **argv);

#endif
