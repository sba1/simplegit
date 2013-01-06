#include <stdlib.h>
#include <unistd.h>
#include "errors.h"
#include "git2.h"
#include "git.h"
#include "git-init.h"
#include "git-support.h"
#include "parse-options.h"
#include "fileops.h"
#include "environment.h"

#define INIT_DB_QUIET 0x0001

static const char *const init_usage[] = {
	"git init' [-q | --quiet] [--bare] [--template=<template_directory>] \
	  [--separate-git-dir|-L <git dir>] \
	  [--shared[=<permissions>]] [directory]",
	NULL
};

int shared_callback(const struct optiong *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	*((int *) opt->value) = 1;

	return 0;
}

int cmd_init(git_repository *dummy, int argc, char **argv)
{
	const char *real_git_dir = NULL;
	const char *template_dir = NULL;
	char *git_dir = NULL;
	unsigned int quiet_flag = 0;
	int is_bare_repository_cfg = 0;
	int init_shared_repository = -1;
	int e;
	int rc = EXIT_FAILURE;

	assert(argc != 0);

	dummy = dummy;

	const struct optiong init_db_options[] = {
		OPT_STRING(0, "template", &template_dir, "template-directory",  "directory from which templates will be used"),
		OPT_SET_INT(0, "bare", &is_bare_repository_cfg, "create a bare repository", 1), {
			OPTION_CALLBACK, 0, "shared", &init_shared_repository,
			"permissions",
			"specify that the git repository is to be shared amongst several users",
			PARSE_OPT_OPTARG | PARSE_OPT_NONEG, shared_callback, 0},
		OPT_BIT('q', "quiet", &quiet_flag, "be quiet", INIT_DB_QUIET),
		OPT_STRING(0, "separate-git-dir", &real_git_dir, "gitdir", "separate git dir from working tree"),
		OPT_END()
	};

	char prefix[] = "";

	argc = parse_options(argc, (const char**)argv, prefix, init_db_options, init_usage, 0);

	if (init_shared_repository != -1) {
		/* Unimplemented : cannot create a shared repository yet */
		please_git_do_it_for_me();
		goto out;
	} else if (real_git_dir) {
		/* Unimplemented : cannot create separate git dir yet */
		please_git_do_it_for_me();
		goto out;
	}

	if (argc == 0) git_dir = ".";
	else if (argc == 1) git_dir = argv[0];
	else
	{
		fprintf(stderr,"Too many arguments!\n");
		goto out;
	}

	git_repository *repo = NULL;

	git_repository_init_options init_opts;
	memset(&init_opts,0,sizeof(init_opts));
	init_opts.version = GIT_REPOSITORY_INIT_OPTIONS_VERSION;
	init_opts.template_path = template_dir;
	init_opts.flags = /*GIT_REPOSITORY_INIT_EXTERNAL_TEMPLATE|*/GIT_REPOSITORY_INIT_NO_REINIT|GIT_REPOSITORY_INIT_MKPATH|((is_bare_repository_cfg)?GIT_REPOSITORY_INIT_BARE:0);

	e = git_repository_init_ext(&repo, git_dir, &init_opts);
	if (e != GIT_OK) goto out;

	if (!quiet_flag)
		printf("Initialized empty Git repository in %s\n", git_repository_path(repo));

	rc = EXIT_SUCCESS;
out:
	if (repo) git_repository_free(repo);
	if (e != GIT_OK)
		libgit_error();
	return rc;
}

