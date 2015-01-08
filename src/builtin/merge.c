/**
 * @file
 */

#include "reset.h"

#include "errors.h"

#include <git2.h>

int cmd_merge(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	const char *commit_str;
	git_reference *commit_ref = NULL;
	git_merge_head *commit_merge_head = NULL;

	git_merge_options merge_options = GIT_MERGE_OPTIONS_INIT;
	git_checkout_options checkout_options = GIT_CHECKOUT_OPTIONS_INIT;

	if (argc < 2)
	{
		fprintf (stderr, "USAGE: %s <commit>\n", argv[0]);
		return -1;
	}

	commit_str = argv[1];

	if ((err = git_branch_lookup(&commit_ref, repo, commit_str, GIT_BRANCH_ALL)))
		goto out;
	if ((err = git_merge_head_from_ref(&commit_merge_head, repo, commit_ref)))
		goto out;

	checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;
	// FIXME: I think that libgit2 prototype is wrong
	if ((err = git_merge(repo, (const git_merge_head **)&commit_merge_head, 1, &merge_options, &checkout_options)))
		goto out;
out:
	if (commit_merge_head) git_merge_head_free(commit_merge_head);
	if (commit_ref) git_reference_free(commit_ref);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
