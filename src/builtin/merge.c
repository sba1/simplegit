/**
 * @file
 */

#include "reset.h"

#include "errors.h"

#include <stdio.h>

#include <git2.h>

int cmd_merge(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	const char *commit_str;
	git_reference *commit_ref = NULL;
	git_annotated_commit *commit_merge_head = NULL;

	git_merge_options merge_options = GIT_MERGE_OPTIONS_INIT;
	git_checkout_options checkout_options = GIT_CHECKOUT_OPTIONS_INIT;

	if (argc < 2)
	{
		fprintf (stderr, "USAGE: %s <commit>\n", argv[0]);
		return -1;
	}

	commit_str = argv[1];

	if ((err = git_branch_lookup(&commit_ref, repo, commit_str, GIT_BRANCH_LOCAL)))
	{
		if (err == GIT_ENOTFOUND)
		{
			if ((err = git_branch_lookup(&commit_ref, repo, commit_str, GIT_BRANCH_REMOTE)))
				goto out;
		} else
		{
			goto out;
		}
	}
	if ((err = git_annotated_commit_from_ref(&commit_merge_head, repo, commit_ref)))
		goto out;

	checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;

	if ((err = git_merge(repo, (const git_annotated_commit **)&commit_merge_head, 1, &merge_options, &checkout_options)))
		goto out;
out:
	if (commit_merge_head) git_annotated_commit_free(commit_merge_head);
	if (commit_ref) git_reference_free(commit_ref);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
