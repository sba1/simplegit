#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-add.h"
#include "strbuf.h"

int cmd_add(git_repository *repo, int argc, char **argv)
{
	int e,i;
	int rc = EXIT_FAILURE;
	const char *wd;
	git_index *idx;
	int num_added = 0;
	int wd_len;

	if (!(wd = git_repository_workdir(repo)))
		goto out;

	wd_len = strlen(wd);

	if ((e = git_repository_index(&idx,repo)) != GIT_OK)
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
					if (git_index_add_from_workdir(idx,path) == 0)
					{
						num_added++;
					} else libgit_error();
				} else fprintf(stderr,"%s is outside repository!\n",argv[i]);
				free(path);
			}
		}
	}

	if (num_added)
		git_index_write(idx);

	git_index_free(idx);

out:
	if (repo) git_repository_free(repo);
	if (e != GIT_OK)
		libgit_error();
	return rc;
}
