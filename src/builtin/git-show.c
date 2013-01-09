#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "git-show.h"
#include "git-log.h"
#include "strbuf.h"
#include "quote.h"

int cmd_show(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int err = 0;
	char *obj_str = NULL;
	git_oid obj_oid;
	git_index *idx = NULL;
	git_object *obj;
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

	if ((err = git_oid_fromstr(&obj_oid,obj_str)) != GIT_OK)
		goto out;

	if ((err = git_object_lookup_prefix(&obj,repo,&obj_oid,strlen(obj_str),GIT_OBJ_ANY)))
		goto out;

	switch ((obj_type = git_object_type(obj)))
	{
		case	GIT_OBJ_COMMIT:
				print_commit((git_commit*)obj);
				break;
		default:
				fprintf(stderr,"Unsupported object type %s\n",git_object_type2string(obj_type));
				goto out;
	}
	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();

	if (idx) git_index_free(idx);

	return rc;
}
