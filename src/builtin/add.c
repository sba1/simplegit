#include "add.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "strbuf.h"

static int cmd_add_matched_paths_callback(const char *path, const char *matched_pathspec, void *payload)
{
	int *num_added = (int*)payload;
	*num_added = *num_added + 1;
	return 0;
}

int cmd_add(git_repository *repo, int argc, char **argv)
{
	int i;
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	git_index *idx = NULL;
	int num_added = 0;
	git_strarray paths;
	const char *wd;

	if (argc < 2)
	{
		fprintf (stderr, "USAGE: %s <paths>\n", argv[0]);
		return -1;
	}

	if (!(wd = git_repository_workdir(repo)))
	{
		fprintf(stderr,"Couldn't determine workdir!\n");
		goto out;
	}

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	paths.count = argc - 1;
	paths.strings = &argv[1];

	/* Check if any path points outside the directory and bailout if so */
	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			/* Use a local buffer for the 2nd arg of realpath() on AmigaOS 4
			 * doesn't accept a NULL pointer for the 2nd arg.
			 */
			char path_buf[PATH_MAX+1];
			char *path;
			if ((path = realpath(argv[i],path_buf)))
			{
				int path_len;

				/* Add directory delimiter if not already there */
				path_len = strlen(path_buf);
				if (path_buf[0] && path_buf[path_len-1] != '/')
				{
					path_buf[path_len] = '/';
					path_buf[path_len+1] = 0;
				}

				if (prefixcmp(path,wd))
				{
					fprintf(stderr,"%s is outside the Git repository\n",argv[i]);
					goto out;
				}
			} else
			{
				fprintf(stderr,"Couldn't determine absolute path of %s: %s!\n",argv[i], strerror(errno));
				goto out;
			}
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if ((err = git_index_add_all(idx, &paths, GIT_INDEX_ADD_DEFAULT, cmd_add_matched_paths_callback, &num_added)))
		goto out;

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
