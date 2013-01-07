#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "repository.h"

#include "git-support.h"

int cmd_update_index(git_repository *repo, int argc, char **argv)
{
	git_index *idx;
	int filec;
	char **filev;
	int rc = EXIT_FAILURE;
	int err = 0;
	int i;
	char complete_path[5000];

	int add_given = 0;
	int remove_given = 0;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i], "--add")) add_given = 1;
		else if (!strcmp(argv[i], "--remove")) remove_given = 1;
		else if (!strcmp(argv[i], "--")) { i++; break; }
		else if (argv[i][0] != '-') break;
		else
		{
			fprintf(stderr,"Unsupported option \"%s\"!\n",argv[i]);
			goto out;
		}
	}

	filec = argc - i;
	filev = &argv[i];
	
	/* Open the index */
	if ((err = git_repository_index(&idx, repo) != GIT_OK))
		goto out;

	for (int i = 0; i < filec; i++)
	{
		int file_exists;
		struct stat st;

		snprintf(complete_path,sizeof(complete_path),"%s%s",get_git_prefix(),filev[i]);

		if (!add_given)
		{
			if (git_index_find(idx, complete_path) < 0)
			{
				printf("error: %s: cannot add to the index - missing --add option?\n", filev[i]);
				printf("fatal: Unable to process path %s\n", filev[i]);
				goto out;
			}
		}

		if (lstat(complete_path,&st) == 0) file_exists = 1;
		else file_exists = 0;

		if (!remove_given && !file_exists)
		{
			printf("error: %s: does not exists and --remove not passed\n", filev[i]);
			printf("fatal: Unable to process path %s\n", filev[i]);
			goto out;
		}

		if (file_exists)
		{
			if ((err = git_index_add_from_workdir(idx, complete_path)) != GIT_OK)
				goto out;
		} else
		{
			if ((err = git_index_remove(idx,complete_path,0)) != GIT_OK)
				goto out;
		}
	}
	if ((err = git_index_write(idx)) != GIT_OK)
		goto out;
	rc = EXIT_SUCCESS;
out:
	if (err != 0)
		libgit_error();
	return rc;
}
