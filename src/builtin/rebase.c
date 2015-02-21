/**
 * @file
 */

#include "reset.h"

#include "common.h"
#include "errors.h"

#include <stdio.h>

#include <git2.h>

int cmd_rebase(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	git_signature *sig = NULL;

	const char *upstream_str;
	git_reference *upstream_ref = NULL;
	git_annotated_commit *upstream = NULL;

	git_reference *branch_ref = NULL;
	git_annotated_commit *branch = NULL;

	git_rebase *rebase = NULL;

	int abort = 0;
	int cont = 0;

	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s <upstream>\n", argv[0]);
		fprintf(stderr, "USAGE: %s --abort|--continue\n", argv[0]);
		goto out;
	}

	upstream_str = argv[1];
	if (!strcmp(upstream_str, "--abort")) abort = 1;
	else if (!strcmp(upstream_str, "--continue")) cont = 1;

	if ((err = sgit_get_author_signature(&sig)))
		goto out;

	if (abort)
	{
		if ((err = git_rebase_open(&rebase, repo)))
			goto out;

		if ((err = git_rebase_abort(rebase, sig)))
			goto out;
	} else
	{
		git_rebase_operation *oper;
		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
		git_rebase_options rebase_opts = GIT_REBASE_OPTIONS_INIT;

		checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

		if (cont)
		{
			if ((err = git_rebase_open(&rebase, repo)))
				goto out;
		} else
		{
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
		}

		while (!(err = git_rebase_next(&oper, rebase, &checkout_opts)))
		{
			git_oid oid;
			if ((err = git_rebase_commit(&oid, rebase, NULL, sig, NULL, NULL)))
				goto out;
		}

		if (err != GIT_ITEROVER && err != GIT_OK)
			goto out;

		if ((err = git_rebase_finish(rebase, sig, &rebase_opts)))
			goto out;
	}
out:
	if (err != GIT_OK)
		libgit_error();
	if (rebase) git_rebase_free(rebase);
	if (upstream) git_annotated_commit_free(upstream);
	if (upstream_ref) git_reference_free(upstream_ref);

	if (branch) git_annotated_commit_free(branch);
	if (branch_ref) git_reference_free(branch_ref);
	if (sig) git_signature_free(sig);
	return rc;
}
