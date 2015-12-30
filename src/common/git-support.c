#include "git-support.h"

#include <stddef.h>
#include <sys/stat.h>

#include <git2.h>

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


static int mergehead_count_cb(const git_oid *oid, void *payload)
{
	int *nparents = (int*)payload;
	*nparents = *nparents + 1;
	return 0;
}

/**
 * Returns the number of merge heads of the given repository.
 *
 * @param num_parents where the number of merge heads will be stored.
 * @param repo the repository to query.
 * @return 0 or an error code. In partular GIT_ENOTFOUND if there are no
 *  merge heads.
 */
int sgit_repository_mergeheads_count(int *num_parents, git_repository *repo)
{
	return git_repository_mergehead_foreach(repo, mergehead_count_cb, num_parents);
}

/**
 * Returns the git mode given the stat mode.
 *
 * @param mode
 * @return
 */
int sgit_get_mode(int st_mode)
{
	if (S_ISLNK(st_mode))
		return S_IFLNK;

	if (S_ISDIR(st_mode) || (st_mode & S_IFMT) == (S_IFLNK | S_IFDIR))
		return (S_IFLNK | S_IFDIR);

	return S_IFREG | ((st_mode & 0100) ? 0755 : 0644);
}

