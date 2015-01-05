#include "show.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "print.h"
#include "strbuf.h"
#include "quote.h"

int cmd_show(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int err = 0;
	char *obj_str = NULL;
	git_object *obj = NULL;
	git_otype obj_type;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"--pretty=raw")) continue;
		else if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option %s!\n",argv[i]);
			goto out;
		} else
		{
			obj_str = argv[i];
		}
	}

	if (!obj_str)
	{
		fprintf(stderr,"Needs a tree argument currently\n");
		goto out;
	}

	if ((err = git_revparse_single(&obj, repo, obj_str)))
		goto out;

	switch ((obj_type = git_object_type(obj)))
	{
		case	GIT_OBJ_COMMIT:
				print_commit((git_commit*)obj,"commit %C\ntree %T\n%p[parent ][\n]Author: %a\nDate:   %d\n\n%m\n\n");
				break;
		default:
				fprintf(stderr,"Unsupported object type %s\n",git_object_type2string(obj_type));
				goto out;
	}
	rc = EXIT_SUCCESS;
out:
	if (obj) git_object_free(obj);

	if (err != GIT_OK)
		libgit_error();

	return rc;
}
