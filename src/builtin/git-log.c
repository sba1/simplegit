#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-log.h"
#include "git-support.h"

int cmd_log(git_repository *repo, int argc, char **argv)
{
	int rc;
	int i;
	git_revwalk *walk;
	git_oid oid;

	rc = EXIT_FAILURE;

	for (i=1;i<argc;i++)
	{
	}

	git_revwalk_new(&walk,repo);
	git_revwalk_push_head(walk);

	while ((git_revwalk_next(&oid, walk)) == 0)
	{
		struct git_commit *wcommit;
		const char *cmsg;
		char c;
		int nl;
		const git_signature *cauth;
		char oid_str[80];
		char t[40];
		time_t tt;
		struct tm tm;

		if (git_commit_lookup(&wcommit, repo, &oid) != 0)
			continue;
		git_oid_tostr(oid_str,sizeof(oid_str),&oid);

		cmsg  = git_commit_message(wcommit);
		cauth = git_commit_author(wcommit);
		tt = cauth->when.time + cauth->when.offset*60;
		tm = *gmtime(&tt);
		tm.tm_gmtoff = cauth->when.offset*60;
		strftime(t,sizeof(t),"%a %d %b %Y %T %z",&tm);

		printf("commit %s\n",oid_str);
		printf("Author: %s <%s>\n",cauth->name,cauth->email);
		printf("Date:   %s\n",t);
		printf("\n");

		nl = 1;
		while ((c = *cmsg++))
		{
			if (nl)
			{
				printf("    ");
				nl = 0;
			}
			putc(c,stdout);
			if (c == '\n') nl = 1;
		}
		printf("\n");
		git_commit_free(wcommit);
	}

	git_revwalk_free(walk);

	rc = EXIT_SUCCESS;
out:
	return rc;
}
