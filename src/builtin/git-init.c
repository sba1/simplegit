#include <stdlib.h>
#include <unistd.h>
#include "git2.h"
#include "git-init.h"
#include "git-support.h"
#include "parse-options.h"
#include "errors.h"

#define INIT_DB_QUIET 0x0001

static const char *const init_usage[] = {
	"git init' [-q | --quiet] [--bare] [--template=<template_directory>] \
	  [--separate-git-dir|-L <git dir>] \
	  [--shared[=<permissions>]] [directory]",
	NULL
};

int shared_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	*((int *) opt->value) = 1;

	return 0;
}

int cmd_init(int argc, const char **argv){
	char cwd[PATH_MAX];
	const char *directory = NULL;
	const char *real_git_dir = NULL;
	const char *template_dir = NULL;
	unsigned int quiet_flag = 0;
	int is_bare_repository_cfg = 0;
	int init_shared_repository = -1;
	
	const struct option init_db_options[] = {
		OPT_STRING(0, "template", &template_dir, "template-directory",
				"directory from which templates will be used"),
		OPT_SET_INT(0, "bare", &is_bare_repository_cfg,
				"create a bare repository", 1),
		{ OPTION_CALLBACK, 0, "shared", &init_shared_repository,
			"permissions",
			"specify that the git repository is to be shared amongst several users",
			PARSE_OPT_OPTARG | PARSE_OPT_NONEG, shared_callback, 0},
		OPT_BIT('q', "quiet", &quiet_flag, "be quiet", INIT_DB_QUIET),
		OPT_STRING(0, "separate-git-dir", &real_git_dir, "gitdir",
			   "separate git dir from working tree"),
		OPT_END()
	};

	char prefix[] = "";

	if (argc != 0) {
		argc = parse_options(argc, argv, prefix, init_db_options, init_usage, 0);
	}

	switch (argc) {
	case 0:
		/* Unimplemented : a git directory must be specified */
		please_git_do_it_for_me();
		directory = getcwd(cwd, PATH_MAX);
		break;
	case 1:
		directory = argv[0];
		break;
	default:
		/* show usage */
		please_git_do_it_for_me();
		break;
	}

	if (template_dir) {
		/* Unimplemented : cannot specify a template directory yet */
		please_git_do_it_for_me();
	} else if (init_shared_repository != -1) {
		/* Unimplemented : cannot create a shared repository yet */
		please_git_do_it_for_me();
	} else if (real_git_dir) {
		/* Unimplemented : cannot create separate git dir yet */
		please_git_do_it_for_me();
	} else if (is_bare_repository_cfg) {
		/* Unimplemented : cannot create bare repository yet */
		please_git_do_it_for_me();
	}

	git_repository *repo;
	int e = git_repository_init (&repo, directory, 0);
	if (e == GIT_ENOTIMPLEMENTED) {
		please_git_do_it_for_me();
	} else if (e) {
		libgit_error();
	}

	if (!quiet_flag) {
		/* For now git2 can only initialize a repository (no reinitialization) */
		printf("Initialized empty Git repository in %s\n", git_repository_path(repo));
	}

	return EXIT_SUCCESS;
}

