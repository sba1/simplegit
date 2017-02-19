#ifndef GIT_FETCH_H
#define GIT_FETCH_H

#include <git2.h>

int progress_cb(const char *str, int len, void *data);
int update_cb(const char *refname, const git_oid *a, const git_oid *b, void *data);

int fetch(git_repository *repo, int argc, char **argv);

#endif
