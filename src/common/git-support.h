#ifndef GIT_SUPPORT_H
#define GIT_SUPPORT_H

#include <git2.h>

int git2_create_branch_from_ref(git_reference **new_branch_ref, git_reference *branch_ref, git_repository* repo, const char *branch);
int sgit_repository_mergeheads_count(int *num_parents, git_repository *repo);
int sgit_get_mode(int st_mode);

#endif
