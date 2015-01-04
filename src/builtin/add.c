#include "add.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "strbuf.h"

int cmd_add(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int i;
	int rc = EXIT_FAILURE;
	const char *wd;
	git_index *idx = NULL;
	int num_added = 0;
	int wd_len;

	if (!(wd = git_repository_workdir(repo)))
	{
		fprintf(stderr,"Couldn't determine workdir!\n");
		goto out;
	}

	wd_len = strlen(wd);

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			char *path;

			if ((path = realpath(argv[i],NULL)))
			{
				if (!prefixcmp(path,wd))
				{
					int path_skip = wd_len;
					while (path[path_skip]=='/') path_skip++;

					if ((err = git_index_add_bypath(idx,&path[path_skip])) != GIT_OK)
						goto out;
					num_added++;
				} else
				{
					fprintf(stderr,"%s is outside repository!\n",argv[i]);
				}
				free(path);
			} else
			{
				fprintf(stderr,"Couldn't determine absolute path of %s!\n",argv[i]);
			}
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if (num_added)
	{
		if ((err = git_index_write(idx)) != GIT_OK)
			goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (idx) git_index_free(idx);
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
