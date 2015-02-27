#ifndef GIT_LS_REMOTE_H
#define GIT_LS_REMOTE_H

#include <git2.h>

int ls_remote(git_repository *repo, int argc, char **argv);

#endif
