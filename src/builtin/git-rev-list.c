#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-rev-list.h"
#include "git-support.h"
#include "parse-options.h"
#include "strbuf.h"

int cmd_rev_list(git_repository *repo, int argc, char **argv)
{
	const char *commit_string = NULL;
	const char *format = NULL;
	int i = 0;
	git_oid commit_oid;
	git_oid current_oid;
	char current_oid_str[GIT_OID_HEXSZ+1];
	int e;
	int rc = EXIT_FAILURE;
	git_commit *commit;
	git_revwalk *walk;

	for (i = 1; i < argc; i++)
	{
		if (*argv[i] == '-')
		{
			if (!prefixcmp(argv[i], "--pretty="))
			{
				format = argv[i] + strlen("--pretty=");
			} else
			{
				fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
				goto out;
			}
		} else
		{
			commit_string = argv[i];
		}
	}

	if (!commit_string)
	{
		fprintf(stderr,"No commit id given!\n");
		goto out;
	}

	if (format)
	{
		fprintf(stderr,"Format \"%s\" not supported!\n",format);
		goto out;
	}

	if ((e = git_oid_fromstrn(&commit_oid,commit_string,strlen(commit_string))) != GIT_OK)
		goto out;

	if ((e = git_commit_lookup_prefix(&commit,repo,&commit_oid,strlen(commit_string))) != GIT_OK)
		goto out;

	if ((e = git_revwalk_new(&walk, repo)) != GIT_OK)
		goto out;

	git_revwalk_sorting(walk, GIT_SORT_TIME);

	if ((e = git_revwalk_push(walk, &commit_oid)) != GIT_OK)
		goto out;

	while ((git_revwalk_next(&current_oid, walk)) == GIT_OK)
	{
		struct git_commit *wcommit;

		if (git_commit_lookup(&wcommit, repo, &current_oid) != 0)
			continue;

		git_oid_tostr(current_oid_str,sizeof(current_oid_str),&current_oid);
		printf("%s\n",current_oid_str);
		git_commit_free(wcommit);
	}

	git_revwalk_free(walk);
out:
	if (e) libgit_error();
	if (commit) git_commit_free(commit);
	return rc;
}
