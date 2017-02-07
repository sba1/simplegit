/**
 * @file
 */

#include "submodule.h"

#include "submodule_parse.c"

#include "errors.h"

#include <stdio.h>

#include <git2.h>

static int submodule_init_cb(git_submodule *sm, const char *name, void *payload)
{
	return git_submodule_init(sm, 0);
}

static int submodule_update_cb(git_submodule *sm, const char *name, void *payload)
{
	git_submodule_update_options opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;
	return git_submodule_update(sm, 0, &opts);
}

int cmd_submodule(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	DocoptArgs args;

	args = docopt(argc, argv, true, NULL);

	if (args.init)
	{
		if ((err = git_submodule_foreach(repo, submodule_init_cb, NULL)))
			goto out;
	} else if (args.update)
	{
		if ((err = git_submodule_foreach(repo, submodule_update_cb, NULL)))
			goto out;
	}
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
