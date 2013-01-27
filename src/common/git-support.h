#ifndef GIT_SUPPORT_H
#define GIT_SUPPORT_H

#include <git2.h>

void please_git_do_it_for_me();
int git2_create_branch_from_ref(git_reference **new_branch_ref, git_reference *branch_ref, git_repository* repo, const char *branch);
int sgit_get_mode(int st_mode);

#endif
