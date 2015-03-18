#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "print.h"
#include "show.h"

int cmd_log(git_repository *repo, int argc, char **argv)
{
	int err = 0;
	int rc;
	git_revwalk *walk;
	git_oid oid;
	git_reference *revision_ref = NULL;
	git_object *revision_obj = NULL;

	rc = EXIT_FAILURE;

	git_revwalk_new(&walk,repo);
	if (argc > 1)
	{
		if ((err = git_reference_dwim(&revision_ref, repo, argv[1])))
			goto out;

		if ((err = git_reference_peel(&revision_obj, revision_ref, GIT_OBJ_ANY)))
			goto out;

		if ((err = git_revwalk_push(walk, git_object_id(revision_obj))))
			goto out;
	} else
	{
		git_revwalk_push_head(walk);
	}

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
	if (err != GIT_OK)
		libgit_error();
	if (revision_obj) git_object_free(revison_obj);
	if (revision_ref) git_reference_free(revision_ref);
	return rc;
}
