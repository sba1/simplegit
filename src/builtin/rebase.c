/**
 * @file
 */

#include "reset.h"

#include "errors.h"

#include <git2.h>

int cmd_rebase(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	const char *upstream_str;
	git_reference *upstream_ref = NULL;
	git_annotated_commit *upstream = NULL;

	git_reference *branch_ref = NULL;
	git_annotated_commit *branch = NULL;

	git_rebase *rebase = NULL;

	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s <upstream>\n", argv[0]);
		goto out;
	}

	if (git_repository_head_detached(repo))
	{
		fprintf(stderr, "Does not work on detached head!\n");
		goto out;
	}

	upstream_str = argv[1];

	if ((err = git_reference_dwim(&upstream_ref, repo, upstream_str)))
		goto out;
	if ((err = git_annotated_commit_from_ref(&upstream, repo, upstream_ref)))
		goto out;

	if ((err = git_repository_head(&branch_ref,repo)) < 0)
		goto out;
	if ((err = git_annotated_commit_from_ref(&branch, repo, branch_ref)))
		goto out;

	if ((err = git_rebase_init(&rebase, repo, branch, upstream, NULL, NULL, NULL)))
		goto out;
out:
	if (err != GIT_OK)
		libgit_error();
	if (rebase) git_rebase_free(rebase);
	if (upstream) git_annotated_commit_free(upstream);
	if (upstream_ref) git_reference_free(upstream_ref);

	if (branch) git_annotated_commit_free(branch);
	if (branch_ref) git_reference_free(branch_ref);
	return rc;
}
