#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"
#include "strbuf.h"
#include "quote.h"

/**
 * Simple path spec comparing function that supports asterisks.
 *
 * @param str
 * @param prefix
 * @return
 */
static int pathspeccmp(const char *str, const char *pspec)
{
	char s,p;

	for (; ; str++, pspec++)
	{
		s = *str;
		p = *pspec;

		if (p == '*')
			return 0;

		if (!p && s == '/')
			return 0;

		if (p != s)
			return (unsigned char)p - (unsigned char)s;

		if (!p)
			return 0;
	}
}

int cmd_ls_files(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int show_cached = 1;
	int err = 0;
	char *file = NULL;
	char buf[GIT_OID_HEXSZ+1];
	git_index *idx = NULL;

	for (i=1;i<argc;i++)
	{
		if (strcmp(argv[i], "--stage") == 0 || strcmp(argv[i], "-s") == 0) show_cached = 0;
		else if (strcmp(argv[i], "--cached") == 0 || strcmp(argv[i], "-c") == 0) show_cached = 1;
		else if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option %s!\n",argv[i]);
			goto out;
		} else
		{
			file = argv[i];
		}
	}

	if (!file) file = "*";

	if ((err = git_repository_index(&idx, repo)) != GIT_OK)
		goto out;

	const char *prefix = "";
	size_t prefix_len = strlen(prefix);

	for (unsigned i = 0; i < git_index_entrycount(idx); i++)
	{
		const git_index_entry *gie = git_index_get_byindex(idx, i);

		if (prefixcmp(gie->path, prefix))
			continue;

		if (pathspeccmp(gie->path,file))
			continue;

		if (!show_cached)
			printf("%06o %s %i\t", gie->mode, git_oid_tostr(buf, GIT_OID_HEXSZ+1, &gie->id), git_index_entry_stage(gie));

		write_name_quoted(gie->path + prefix_len, stdout, '\n');
	}

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();

	if (idx) git_index_free(idx);

	return rc;
}
