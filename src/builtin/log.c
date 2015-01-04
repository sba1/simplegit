#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-support.h"
#include "print.h"
#include "show.h"

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

		if (git_commit_lookup(&wcommit, repo, &oid) != 0)
			continue;

		print_commit(wcommit,"commit %C\nAuthor: %a\nDate:   %d\n\n%m\n");

		git_commit_free(wcommit);
	}

	git_revwalk_free(walk);

	rc = EXIT_SUCCESS;
out:
	return rc;
}
