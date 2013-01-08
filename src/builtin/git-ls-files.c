#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "strbuf.h"
#include "quote.h"

int cmd_ls_files(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int show_cached = 1;
	int err;
	char *file = NULL;
	git_index *idx;

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

	if ((err = git_repository_index(&idx, repo)) != GIT_OK)
		goto out;

	char buf[GIT_OID_HEXSZ+1];
	const char *prefix = get_git_prefix();
	size_t prefix_len = strlen(prefix);

	for (unsigned i = 0; i < git_index_entrycount(idx); i++)
	{
		const git_index_entry *gie = git_index_get_byindex(idx, i);

		if (prefixcmp(gie->path, prefix))
			continue;

		if (file && strcmp(gie->path,file)) continue;

		if (!show_cached)
			printf("%06o %s %i\t", gie->mode, git_oid_tostr(buf, GIT_OID_HEXSZ+1, &gie->oid), git_index_entry_stage(gie));

		write_name_quoted(gie->path + prefix_len, stdout, '\n');
	}

	git_index_free(idx);

	rc = EXIT_SUCCESS;

out:
	if (err) libgit_error();
	return rc;
}
