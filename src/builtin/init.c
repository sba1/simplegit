#include "init.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "errors.h"
#include "git2.h"
#include "git.h"
#include "git-support.h"
#include "environment.h"

static int getarg(int argc, char **argv, int *argpos, char *arg, const char **value)
{
	int arglen;

	if (prefixcmp(argv[*argpos],arg))
		return 0;

	arglen = strlen(arg);
	if (argv[*argpos][arglen]=='=')
	{
		*value = &argv[*argpos][arglen+1];
	} else
	{
		if (*argpos + 1 == argc)
			return 0;
		*argpos = *argpos + 1;
		*value = argv[*argpos];
	}

	return 1;
}

int cmd_init(git_repository *dummy, int argc, char **argv)
{
	const char *template_dir = NULL;
	char *git_dir = NULL;
	unsigned int quiet_flag = 0;
	int is_bare_repository_cfg = 0;
	git_repository *repo = NULL;
	int err = GIT_OK;
	int i;
	int rc = EXIT_FAILURE;

	dummy = dummy;

	for (i=1;i<argc;i++)
	{
		if (getarg(argc,argv,&i,"--template",&template_dir)) continue;
		if (!strcmp("--quiet",argv[i]))
		{
			quiet_flag = 1;
		} else if (!strcmp("--bare",argv[i]))
		{
			is_bare_repository_cfg = 1;
		} else if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		} else
		{
			if (!git_dir) git_dir = argv[i];
			else
			{
				fprintf(stderr,"Only one directory can be given\n");
				goto out;
			}
		}

	}

	if (!git_dir) git_dir = ".";
	if (template_dir && !*template_dir) template_dir = ".";

	git_repository_init_options init_opts;
	memset(&init_opts,0,sizeof(init_opts));
	init_opts.version = GIT_REPOSITORY_INIT_OPTIONS_VERSION;
	init_opts.template_path = template_dir;
	init_opts.flags = (template_dir?GIT_REPOSITORY_INIT_EXTERNAL_TEMPLATE:0)|GIT_REPOSITORY_INIT_NO_REINIT|GIT_REPOSITORY_INIT_MKPATH|((is_bare_repository_cfg)?GIT_REPOSITORY_INIT_BARE:0);

	err = git_repository_init_ext(&repo, git_dir, &init_opts);
	if (err != GIT_OK) goto out;

	if (!quiet_flag)
		printf("Initialized empty Git repository in %s\n", git_repository_path(repo));

	rc = EXIT_SUCCESS;
out:
	if (repo) git_repository_free(repo);
	if (err != GIT_OK)
		libgit_error();
	return rc;
}

