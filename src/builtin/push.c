#include "push.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "common.h"
#include "errors.h"
#include "strbuf.h"

static int push_cred_acquire_callback(
        git_cred **cred,
        const char *url,
        const char *username_from_url,
        unsigned int allowed_types,
        void *payload)
{
	char buf[100];
	char username[40];
	char *passwd;

	int i;

	if (prefixcmp(url,"https:"))
		return -1;

	printf("Enter user name for %s: ",url);
	fgets(username,sizeof(username),stdin);

	snprintf(buf,sizeof(buf),"Enter password for %s: ",url);
	passwd = getpass(buf);

	for (i = strlen(username) - 1; i>=0; i--)
		if (username[i]=='\n')
			username[i] = 0;

	if (git_cred_userpass_plaintext_new(cred,username,passwd) != GIT_OK)
		return -1;

	return 0;
}

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

	callbacks.credentials = push_cred_acquire_callback;
	callbacks.push_update_reference = push_update_reference_callback;
	callbacks.certificate_check = certificate_check;
	git_remote_set_callbacks(r, &callbacks);
	if ((err = git_remote_push(r, &refs, &push_options, NULL, NULL)))
		goto out;

out:
	if (err != GIT_OK)
		libgit_error();
	free(ref_fullname);
	if (r) git_remote_free(r);
	return rc;
}
