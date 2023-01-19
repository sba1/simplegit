/**
 * @file
 */

#include "stash.h"

#include "errors.h"

#include <stdio.h>

#include <git2.h>

static void print_usage(char *name)
{
	fprintf (stderr, "USAGE: %s [apply|list|pop]\n", name);
}

static int stash_cb(size_t index, const char* message, const git_oid *stash_id, void *payload)
{
	printf("stash@{%d}: %s\n", index, message);
	return 0;
}

static int top_stash_cb(size_t index, const char* message, const git_oid *stash_id, void *payload)
{
	if (index != 0)
		return 0;

	*(git_oid*)payload = *stash_id;
	return 1;
}

int cmd_stash(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	int pop = 0;
	int list = 0;
	int apply = 0;

	if (argc == 2)
	{
		if (!strcmp("pop", argv[1])) pop = 1;
		else if (!strcmp("list", argv[1])) list = 1;
		else if (!strcmp("apply", argv[1])) apply = 1;
		else
		{
			print_usage(argv[0]);
			goto out;
		}
	} else if (argc != 1)
	{
		print_usage(argv[0]);
		goto out;
	}

	if (!apply && !pop && !list)
	{
		git_oid oid;
		git_signature stasher = {0};
		stasher.email = "stasher@stasher.dddd";

		if ((err = git_stash_save(&oid, repo, &stasher, NULL, 0)))
			goto out;
	} else if (list)
	{
		if ((err = git_stash_foreach(repo, stash_cb, NULL)))
			goto out;
	} else if (apply || pop)
	{
		git_oid oid;
		git_commit *commit;
		git_tree *tree;
		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

		if ((err = git_stash_foreach(repo, top_stash_cb, &oid)) < 0)
			goto out;

		if ((err = git_commit_lookup(&commit, repo, &oid)))
			goto out;

		if ((err = git_commit_tree(&tree, commit)))
			goto out;

		checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE|GIT_CHECKOUT_DONT_UPDATE_INDEX;
		if ((err = git_checkout_tree(repo, (git_object*)tree, &checkout_opts)))
			goto out;

		if (pop)
		{
			if ((err = git_stash_drop(repo, 0)))
				goto out;
		}
	}

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
