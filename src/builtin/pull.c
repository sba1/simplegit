#include "pull.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <git2.h>

#include "fetch.h"
#include "merge.h"

#include "strbuf.h"

#include "cli/pull_cli.c"

int cmd_pull(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	struct cli cli = {0};
	char *fetch_args[2] = {NULL};
	char *merge_args[2] = {NULL};
	char merge_ref_spec[96];

	if (!parse_cli(argc, argv, &cli, POF_VALIDATE))
	{
		return GIT_ERROR;
	}

	if (usage_cli(argv[0], &cli))
	{
		return GIT_OK;
	}

	/* fetch */
	fetch_args[0] = "fetch";
	fetch_args[1] = cli.repository;

	if (fetch(repo, 2, fetch_args) != EXIT_SUCCESS)
		goto out;

	/* merge */
	merge_args[0] = "merge";
	snprintf(merge_ref_spec, sizeof(merge_ref_spec), "%s/%s", cli.repository, cli.refspec);
	merge_args[1] = merge_ref_spec;
	if (cmd_merge(repo, 2, merge_args) != EXIT_SUCCESS)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
