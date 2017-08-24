/**
 * @file
 */

#include "submodule.h"

#include "cli/submodule_cli.c"

#include "common.h"
#include "fetch.h"
#include "errors.h"

#include <stdio.h>

#include <git2.h>

static int submodule_init_cb(git_submodule *sm, const char *name, void *payload)
{
	(void)name;
	(void)payload;

	return git_submodule_init(sm, 0);
}

static int submodule_update_cb(git_submodule *sm, const char *name, void *payload)
{
	(void)payload;

	git_submodule_update_options opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;
	git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

	callbacks.update_tips = update_cb;
	callbacks.sideband_progress = progress_cb;
	callbacks.credentials = cred_acquire_cb;
	callbacks.certificate_check = certificate_check;

	opts.fetch_opts.callbacks = callbacks;

	return git_submodule_update(sm, 0, &opts);
}

int cmd_submodule(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	struct cli cli = {0};

	if (!parse_cli(argc - 1, &argv[1], &cli, POF_VALIDATE))
	{
		return GIT_ERROR;
	}

	if (usage_cli(argv[0], &cli))
	{
		return GIT_OK;
	}


	if (cli.init)
	{
		if ((err = git_submodule_foreach(repo, submodule_init_cb, NULL)))
			goto out;
	} else if (cli.update)
	{
		if ((err = git_submodule_foreach(repo, submodule_update_cb, NULL)))
			goto out;
	}
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
