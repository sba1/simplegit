#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"

#include "cli/config_cli.c"

int cmd_config(git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = 0;

	git_config *config = NULL;

	struct cli cli = {0};

	if (!parse_cli(argc, argv, &cli, POF_VALIDATE))
	{
		return GIT_ERROR;
	}

	if (usage_cli(argv[0], &cli))
	{
		return GIT_OK;
	}

	if (repo && !cli.global)
	{
		if (!cli.value)
		{
			if ((err = git_repository_config_snapshot(&config,repo)) != GIT_OK)
				goto out;
		} else
		{
			if ((err = git_repository_config(&config,repo)) != GIT_OK)
				goto out;
		}
	} else
	{
#ifdef __amigaos4__
		FILE *f = fopen("ENVARC:gitconfig", "rb");
		if (!f)
		{
			if ((f = fopen("ENVARC:gitconfig", "wb")))
			{
				fprintf(stderr, "Created global config in ENVARC:gitconfig\n");
			} else
			{
				fprintf(stderr, "Unable to create ENVARC:gitconfig\n");
			}
		}
		if (f)
		{
			fclose(f);
		}
#endif
		if ((err = git_config_open_default(&config)))
			goto out;

		if (!cli.value)
		{
			git_config *snapshot;

			if ((err = git_config_snapshot(&snapshot, config)))
				goto out;

			git_config_free(config);
			config = snapshot;
		}
	}

	if (!cli.name)
	{
		fprintf(stderr,"Invalid or not supported command line format!\n");
		goto out;
	}

	if (cli.value)
	{
		if ((err = git_config_set_string(config, cli.name, cli.value)))
			goto out;
	} else
	{
		const char *val;

		if ((err = git_config_get_string(&val, config, cli.name)) != GIT_OK)
			goto out;
		printf("%s\n",val);
	}

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (config) git_config_free(config);

	return rc;
}
