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

	git_merge_analysis_t analysis;
	git_merge_preference_t preference;

	const char *commit_str;
	git_reference *commit_ref = NULL;
	git_object *commit_obj = NULL;
	git_annotated_commit *commit_merge_head = NULL;

	git_reference *head_ref = NULL;

	git_tree *commit_tree_obj = NULL;
	git_reference *new_ref = NULL;

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

	if ((err = git_merge_analysis(&analysis, &preference, repo, (const git_annotated_commit **)&commit_merge_head, 1)))
		goto out;

	checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;

	if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD && preference != GIT_MERGE_PREFERENCE_NO_FASTFORWARD)
	{
		fprintf(stderr, "Fast forward merge\n");

		if ((err = git_reference_peel(&commit_obj, commit_ref, GIT_OBJ_COMMIT)))
			goto out;

		if ((err = git_repository_head(&head_ref,repo)))
			goto out;

		if ((err = git_commit_tree(&commit_tree_obj,(git_commit*)commit_obj)))
			goto out;

		if ((err = git_checkout_tree(repo, (git_object*)commit_tree_obj, &checkout_options)))
			goto out;

		if ((err = git_reference_set_target(&new_ref, head_ref, git_commit_id(commit_obj), NULL, NULL)))
			goto out;

		goto out;
	}

	if ((err = git_merge(repo, (const git_annotated_commit **)&commit_merge_head, 1, &merge_options, &checkout_options)))
		goto out;
out:
	if (commit_merge_head) git_annotated_commit_free(commit_merge_head);
	if (commit_obj) git_object_free(commit_obj);
	if (commit_ref) git_reference_free(commit_ref);
	if (head_ref) git_reference_free(head_ref);
	if (commit_tree_obj) git_tree_free(commit_tree_obj);
	if (new_ref) git_reference_free(new_ref);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
