/**
 * @file
 */

#include "reset.h"

#include "commit.h"
#include "errors.h"

#include <stdio.h>

#include <git2.h>

int cmd_merge(git_repository *repo, int argc, char **argv)
{
	int i;
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	int autocommit = 1;
	int ff_only = 0;

	git_merge_analysis_t analysis;
	git_merge_preference_t preference;

	const char *commit_str = NULL;
	git_reference *commit_ref = NULL;
	git_object *commit_obj = NULL;
	git_annotated_commit *commit_merge_head = NULL;

	git_reference *head_ref = NULL;

	git_tree *commit_tree_obj = NULL;
	git_reference *new_ref = NULL;

	git_merge_options merge_options = GIT_MERGE_OPTIONS_INIT;
	git_checkout_options checkout_options = GIT_CHECKOUT_OPTIONS_INIT;

	git_index *index = NULL;

	if (argc < 2)
	{
		fprintf (stderr, "USAGE: %s <commit> [--no-commit] [--ff-only]\n", argv[0]);
		return -1;
	}

	for (i=1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--no-commit")) autocommit = 0;
		else if (!strcmp(argv[i], "--ff-only")) ff_only = 1;
		else if (!commit_str) commit_str = argv[i];
		else
		{
			fprintf (stderr, "USAGE: %s <commit> [--no-commit]\n", argv[0]);
			goto out;
		}
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

	checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE;

	if (ff_only && !(analysis & GIT_MERGE_ANALYSIS_FASTFORWARD))
	{
		fprintf(stderr, "No fast forward merge possible\n");
		goto out;
	}

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

		if ((err = git_reference_set_target(&new_ref, head_ref, git_commit_id((git_commit*)commit_obj), NULL)))
			goto out;

		goto out;
	}

	if ((err = git_merge(repo, (const git_annotated_commit **)&commit_merge_head, 1, &merge_options, &checkout_options)))
		goto out;

	if ((err = git_repository_index(&index, repo)))
		goto out;

	if (!git_index_has_conflicts(index))
	{
		if (autocommit)
		{
			char *argv[3];
			char message[256];
			snprintf(message, sizeof(message),"Merged branch '%s'",commit_str);
			argv[0] = "commit";
			argv[1] = "-m";
			argv[2] = message;
			cmd_commit(repo, 3, argv);
		}
	} else
	{
		printf("conflict during merge! Please resolve and commit\n");
	}
out:
	if (index) git_index_free(index);
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
