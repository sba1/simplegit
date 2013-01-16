#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "errors.h"
#include "git-push.h"
#include "strbuf.h"

static int push_status_callback(const char *ref, const char *msg, void *data)
{
	printf("%p\n",msg);
	return 0;
}

static int push_cred_acquire_callback(git_cred **cred, const char *url, unsigned int allowed_types, void *payload)
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

int cmd_push(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int i;
	int rc = EXIT_FAILURE;

	git_push *p = NULL;
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
			fprintf(stderr,"Only one argument supported for now!\n");
			goto out;
		}
		if ((err = git_remote_load(&r,repo,argv[i])) != GIT_OK)
			goto out;
	}

	if (!r)
	{
		fprintf(stderr,"No remote given!\n");
		goto out;
	}

	git_remote_set_cred_acquire_cb(r,push_cred_acquire_callback,NULL);

	if ((err = git_push_new(&p,r)) != GIT_OK)
		goto out;

	if ((err = git_push_status_foreach(p,push_status_callback,NULL)) != GIT_OK)
		goto out;

	if ((err = git_push_add_refspec(p,"refs/heads/master")) != GIT_OK)
		goto out;

	if ((err = git_push_finish(p)) != GIT_OK)
		goto out;
out:
	if (err != GIT_OK)
		libgit_error();
	if (p) git_push_free(p);
	if (r) git_remote_free(r);
	return rc;
}
