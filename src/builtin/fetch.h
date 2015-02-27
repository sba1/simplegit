#ifndef GIT_FETCH_H
#define GIT_FETCH_H

#include <git2.h>

int fetch(git_repository *repo, int argc, char **argv);

#endif
