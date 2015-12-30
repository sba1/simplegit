#include "push.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "common.h"
#include "errors.h"
#include "strbuf.h"

static int push_update_reference_callback(const char *refname, const char *status, void *data)
{
	printf("%s: %s\n", refname, status?status:"Ok");
	return 0;
}

int cmd_push(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int i;
	int rc = EXIT_FAILURE;
	git_strarray refs = {NULL, 0};
	git_push_options push_options = GIT_PUSH_OPTIONS_INIT;
	git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
	char *ref_fullname = NULL;

	git_remote *r = NULL;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}

		if (r)
		{
			git_reference *ref;

			if (refs.count) {
				fprintf(stderr, "USAGE: %s <remote> <refspec>\n", argv[0]);
				goto out;
			}

			if (!git_reference_dwim(&ref, repo, argv[i]))
			{
				ref_fullname = strdup(git_reference_name(ref));
				refs.strings = &ref_fullname;
				git_reference_free(ref);
			} else
			{
				refs.strings = &argv[i];
			}
			refs.count = 1;
		} else
		{
			if ((err = git_remote_lookup(&r,repo,argv[i])) != GIT_OK)
				goto out;
		}
	}

	if (!r)
	{
		fprintf(stderr,"No remote given!\n");
		goto out;
	}

	if (refs.count == 0)
	{
		git_strarray cfg_refs;

		if ((err = git_remote_get_push_refspecs(&cfg_refs, r)))
			goto out;

		if (!cfg_refs.count)
		{
			fprintf(stderr, "No refspec given and no refspec configured. "
					"Pushing is probably a noop.\n");
		}
		git_strarray_free(&cfg_refs);
	}
	callbacks.credentials = cred_acquire_cb;
	callbacks.push_update_reference = push_update_reference_callback;
	callbacks.certificate_check = certificate_check;
	push_options.callbacks = callbacks;

	if ((err = git_remote_push(r, &refs, &push_options)))
		goto out;

out:
	if (err != GIT_OK)
		libgit_error();
	free(ref_fullname);
	if (r) git_remote_free(r);
	return rc;
}
