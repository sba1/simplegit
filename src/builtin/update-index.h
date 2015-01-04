#ifndef GIT_UPDATE_INDEX_H
#define GIT_UPDATE_INDEX_H

#include <git2.h>

int cmd_update_index(git_repository *repo, int argc, char **argv);

#endif
