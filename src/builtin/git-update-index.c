#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "repository.h"
#include "strbuf.h"
#include "git-support.h"

/**
 * Returns whether the given path is part of a directory in the given index.
 *
 * @param idx
 * @param file
 * @return
 */
static int does_path_overlap_with_a_directory_in_index(git_index *idx, char *path)
{
	size_t i;
	size_t entries;

	/* If the path is contained in the index, then by assumption,
	 * it cannot be a directory.
	 */
	if (git_index_get_bypath(idx, path, 0))
		return 0;

	entries = git_index_entrycount(idx);

	/* Slow because we have to check each entry */
	for (i=0;i<entries;i++)
	{
		const git_index_entry *ie;

		ie = git_index_get_byindex(idx,i);

		if (!prefixcmp(ie->path,path) && ie->path[strlen(path)] == '/')
			return 1;
	}

	return 0;
}

/**
 * Returns the index entry that is a prefix of the given path.
 *
 * @param idx
 * @param path
 * @return the index entry or NULL.
 */
static const git_index_entry *find_index_entry_that_is_a_prefix_of_path(git_index *idx, char *path)
{
	size_t i;
	size_t entries;

	entries = git_index_entrycount(idx);

	/* Slow because we have to check each entry */
	for (i=0;i<entries;i++)
	{
		const git_index_entry *ie;

		ie = git_index_get_byindex(idx,i);

		if (!prefixcmp(path,ie->path) && path[strlen(ie->path)] == '/')
			return ie;
	}

	return NULL;
}

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
	int replace_given = 0;
	int index_info_given = 0;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i], "--add")) add_given = 1;
		else if (!strcmp(argv[i], "--remove")) remove_given = 1;
		else if (!strcmp(argv[i], "--replace")) replace_given = 1;
		else if (!strcmp(argv[i], "--index-info")) index_info_given = 1;
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
		int file_exists_in_wd;
		int file_exists_in_index;
		struct stat st;

		snprintf(complete_path,sizeof(complete_path),"%s%s",get_git_prefix(),filev[i]);
		file_exists_in_index = !!git_index_get_bypath(idx, complete_path, 0);

		if (lstat(complete_path,&st) == 0) file_exists_in_wd = 1;
		else file_exists_in_wd = 0;

		if (!add_given && !file_exists_in_index)
		{
			printf("error: %s: cannot add to the index - missing --add option?\n", filev[i]);
			printf("fatal: Unable to process path %s\n", filev[i]);
			goto out;
		}

		if (!remove_given && !file_exists_in_wd)
		{
			printf("error: %s: does not exists and --remove not passed\n", filev[i]);
			printf("fatal: Unable to process path %s\n", filev[i]);
			goto out;
		}

		if (file_exists_in_wd)
		{
			const git_index_entry *ie;

			if ((!(st.st_mode & __S_IFDIR)) && does_path_overlap_with_a_directory_in_index(idx,complete_path))
			{
				if (!replace_given)
				{
					fprintf(stderr,"File %s is contained as a directory in the index!\n",complete_path);
					goto out;
				}
				if ((err = git_index_remove_directory(idx,complete_path,0)) != GIT_OK)
					goto out;
			}

			if ((ie = find_index_entry_that_is_a_prefix_of_path(idx,complete_path)))
			{
				if (!replace_given)
				{
					fprintf(stderr,"The path %s extends a file in the index!\n",complete_path);
					goto out;
				}
				if ((err = git_index_remove(idx,ie->path,0)) != GIT_OK)
					goto out;
			}
		}

		if (file_exists_in_wd)
		{
			if ((err = git_index_add_from_workdir(idx, complete_path)) != GIT_OK)
				goto out;
		} else
		{
			if ((err = git_index_remove(idx,complete_path,0)) != GIT_OK)
				goto out;
		}
	}

	if (index_info_given)
	{
		char line[5000];

		/* Read the index information from stdin */

		while (fgets(line,sizeof(line),stdin))
		{
			git_index_entry e;
			int pos;
			char c;
			int line_len = strlen(line);
			int is_ls_files_format = 0;

			if (line_len<=52)
				continue;

			memset(&e,0,sizeof(e));

			/* There seem to be multiple formats that index-infos accepts.
			 * TODO: Implement this more elegantly */
			e.mode = strtol(line,NULL,8);

			if (git_oid_fromstrn(&e.oid,&line[12],40) == -1)
			{
				if (git_oid_fromstrn(&e.oid,&line[7],40) == -1)
				{
					fprintf(stderr,"Invalid OID format!\n");
					continue;
				}
				is_ls_files_format = 1;
			}

			if (is_ls_files_format) pos = 50;
			else pos = 52;

			while ((c = line[pos]))
			{
				if (!isspace((unsigned char)c))
					break;
				pos++;
			}
			if (!line[pos])
			{
				continue;
			}

			if (line[line_len-1] == '\n') line[line_len-1] = 0;

			e.path = &line[pos];

			if ((err = git_index_add(idx,&e)) != GIT_OK)
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
