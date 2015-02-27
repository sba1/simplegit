#ifndef GIT_INDEX_PACK_H
#define GIT_INDEX_PACK_H

#include <git2.h>

int index_pack(git_repository *repo, int argc, char **argv);

#endif
