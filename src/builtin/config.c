#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"

int cmd_config(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int err = 0;
	int global = 0;

	git_config *config = NULL;

	const char *name = NULL;
	const char *val = NULL;

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--global"))
		{
			global = 1;
			break;
		}
	}

	if (repo && !global)
	{
		if ((err = git_repository_config(&config,repo)) != GIT_OK)
			goto out;
	} else
	{
		if ((err = git_config_open_default(&config)))
			goto out;
	}

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			if (!name) name = argv[i];
			else if (!val) val = argv[i];
			else
			{
				fprintf(stderr,"Invalid or not supported command line format!\n");
				goto out;
			}
		}
	}

	if (!name)
	{
		fprintf(stderr,"Invalid or not supported command line format!\n");
		goto out;
	}

	if (val)
	{
		if ((err = git_config_set_string(config, name, val)))
			goto out;
	} else
	{
		if ((err = git_config_get_string(&val,config,name)) != GIT_OK)
			goto out;
		printf("%s\n",val);
	}

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (config) git_config_free(config);

	return rc;
}
