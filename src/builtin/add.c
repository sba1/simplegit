#include "add.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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
	int root_added = 0;
	git_strarray paths;
	const char *wd;
	int wd_len;
	DIR *dir = NULL;

	char **relative_paths = NULL;
	int num_relative_paths = 0;

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
	wd_len = strlen(wd);

	if (!(relative_paths = (char**)malloc(sizeof(*relative_paths)*(argc - 1))))
	{
		fprintf(stderr,"Memory allocation failed!\n");
		goto out;
	}
	memset(relative_paths, 0, sizeof(*relative_paths)*(argc - 1));

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

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

				if (strlen(&path[wd_len]))
				{
					if (!(relative_paths[num_relative_paths] = strdup(&path[wd_len])))
					{
						fprintf(stderr,"Memory allocation failed!\n");
						goto out;
					}
					num_relative_paths++;
				} else
				{
					/* Remember that the root dir have been added.
					 * TODO: Do we really have to add the other paths then? */
					root_added = 1;
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

	if (num_relative_paths)
	{
		paths.count = num_relative_paths;
		paths.strings = relative_paths;

		if ((err = git_index_add_all(idx, &paths, GIT_INDEX_ADD_DEFAULT, cmd_add_matched_paths_callback, &num_added)))
			goto out;
	}

	if (root_added)
	{
		/* At least in libgit 0.20.3 adding the root directory didn't work.
		 * We have to add the directories manually.
		 */
		struct dirent *dirent;
		if (!(dir = opendir(wd)))
		{
			fprintf(stderr,"Can browse root of repository!\n");
			goto out;
		}

		while ((dirent = readdir(dir)))
		{
			int ignore;
			char *dirname;

			if (!strcmp(dirent->d_name,".") || !strcmp(dirent->d_name,".."))
				continue;

			if ((err = git_ignore_path_is_ignored(&ignore, repo, dirent->d_name)))
				goto out;

			if (ignore) continue;

			dirname = dirent->d_name;
			paths.count = 1;
			paths.strings = &dirname;

			if ((err = git_index_add_all(idx, &paths, GIT_INDEX_ADD_DEFAULT, cmd_add_matched_paths_callback, &num_added)))
				goto out;
		}

		closedir(dir);
		dir = NULL;
	}

	if (num_added)
	{
		if ((err = git_index_write(idx)) != GIT_OK)
			goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (dir) closedir(dir);
	if (relative_paths)
	{
		int i;
		for (i = 0; i < num_relative_paths; i++)
			free(relative_paths[i]);

		free(relative_paths);
	}
	if (idx) git_index_free(idx);
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
