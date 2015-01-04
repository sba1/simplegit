#ifndef GIT_MKTAG_H
#define GIT_MKTAG_H

#include <git2.h>

int cmd_mktag(git_repository *repo, int argc, char **argv);

#endif
