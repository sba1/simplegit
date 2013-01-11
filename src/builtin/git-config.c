#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "git-config.h"

int cmd_config(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int err = 0;

	for (i=1;i<argc;i++)
	{
		goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();

	return rc;
}
