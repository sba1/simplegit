/**
 * @file
 */

#include "cherry-pick.h"

#include <stdio.h>
#include <git2.h>

static void print_usage(char *name)
{
	fprintf (stderr, "USAGE: %s <commit>\n", name);
}

int cmd_cherry_pick(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	git_cherrypick_options opts = GIT_CHERRYPICK_OPTIONS_INIT;
	git_oid commit_oid;
	git_commit *commit;

	if (argc > 2)
	{
		print_usage(argv[0]);
		goto out;
	}

	if ((err = git_oid_fromstrp(&commit_oid,argv[1])) != GIT_OK)
		goto out;

	if ((err = git_commit_lookup_prefix(&commit, repo, &commit_oid, strlen(argv[1]))))
		goto out;

	if ((err = git_cherrypick(repo, commit, &opts)))
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
