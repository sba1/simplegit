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

int cmd_init(git_repository *dummy, int argc, char **argv){
	/* Some tests won't pass because of template dirs
	 * config files or shared repository.
	 * Other than that, it works fine.
	 */
	please_git_do_it_for_me();

	char cwd[PATH_MAX];
	const char *real_git_dir = NULL;
	const char *template_dir = NULL;
	char default_git_dir[] = "";
	char *git_dir = NULL;
	unsigned int quiet_flag = 0;
	int is_bare_repository_cfg = 0;
	int init_shared_repository = -1;
	int e;

	assert(argc != 0);
	
	const struct optiong init_db_options[] = {
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

	argc = parse_options(argc, argv, prefix, init_db_options, init_usage, 0);

	if (argc > 1) {
		/* invalid use : show usage */
		please_git_do_it_for_me();
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
	}

	if (getenv(GIT_OBJECT_DIR_ENVIRONMENT)) {
		please_git_do_it_for_me();
	} else if (getenv(GIT_TEMPLATE_DIR_ENVIRONMENT)) {
		please_git_do_it_for_me();
	}

	git_config *cfg;
	e = git_config_open_default(&cfg);
	if (e == GIT_OK) {
		const char *init_template_dir;
		/* libgit2 does not handle template dirs for now */
		e = git_config_get_string(&init_template_dir, cfg, "init.templatedir");
		if (e == GIT_OK && init_template_dir != NULL) {
			please_git_do_it_for_me();
		}

		git_config_free(cfg);
	}

	if (argc == 1) {
		e = git2_prettify_dir_path(cwd, PATH_MAX, argv[0]);
		if (e) error("Given directory path is invalid\n");

		if (git2_exists(cwd)) {
			git2_mkdir_recurs(cwd, 0755);
		} else if (git2_isdir(cwd)) {
			error("Given path is not a directory !\n");
		}

		git2_chdir(cwd);
	} else if (argc > 1) {
		/* show usage */
		please_git_do_it_for_me();
	}

	git_repository *repo;

	git_dir = getenv(GIT_DIR_ENVIRONMENT);
	if (!git_dir)
		git_dir = default_git_dir;
	else
		is_bare_repository_cfg = 1;

	e = git_repository_init(&repo, git_dir, is_bare_repository_cfg);

	if (e != GIT_OK) {
		libgit_error();
	}

	if (!quiet_flag) {
		/* For now git2 can only initialize a repository (no reinitialization) */
		printf("Initialized empty Git repository in %s\n", git_repository_path(repo));
	}

	git_repository_free(repo);
	return EXIT_SUCCESS;
}

