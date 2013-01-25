#include <unistd.h>
#include "git-support.h"
#include "utils.h"
#include "errors.h"

void please_git_do_it_for_me()
{
	fprintf(stderr,"please_git_do_it_for_me()\n");
	exit(20);
}

/**
 * Create a branch with a given branch name from a reference.
 *
 * @param new_branch_ref where the reference of the new branch is stored to.
 * @param branch_ref the ref which to branch.
 * @param repo the repo
 * @param branch the name of the new branch
 * @return error code or 0 for success.
 */
int git2_create_branch_from_ref(git_reference **new_branch_ref, git_reference *branch_ref, git_repository* repo, const char *branch)
{
	int err;

	git_object *commit;
	git_commit *comm;

	if ((err = git_reference_peel(&commit, branch_ref, GIT_OBJ_COMMIT)) != 0)
		goto out;

	if ((err = git_commit_lookup(&comm, repo, git_object_id(commit))) != 0)
		goto out;

	err = git_branch_create(new_branch_ref, repo, branch, comm, 0);
out:
	return err;
}
