#include <stdlib.h>
#include "git2.h"
#include "git-init.h"
#include "git-support.h"
#include "parse-options.h"
#include "errors.h"

#define INIT_DB_QUIET 0x0001

static const char *const init_usage[] = {
	"git init [-q | --quiet] [--bare] [--template=<template-directory>] [--shared[=<permissions>]] [directory]",
	NULL
};

int cmd_init(int argc, const char **argv){
	// WORK IN PROGRESS
	// TODO : delete this line when finished
	please_git_do_it_for_me();




	const char *real_git_dir = NULL;
	const char *template_dir = NULL;
	unsigned int flags = 0;
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
			PARSE_OPT_OPTARG | PARSE_OPT_NONEG, NULL/*shared_callback*/, 0},
		OPT_BIT('q', "quiet", &flags, "be quiet", INIT_DB_QUIET),
		OPT_STRING('L', "separate-git-dir", &real_git_dir, "gitdir",
			   "separate git dir from working tree"),
		OPT_END()
	};

	char prefix[1] = {0};
	argc = parse_options(argc, argv, prefix, init_db_options, init_usage, 0);

	if (argc == 0) {
		/* Unimplemented : a directory must be specified for now */
		please_git_do_it_for_me();
	} else {
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
		int e = git_repository_init (&repo, argv[argc-1], 0);
		if (e == GIT_ENOTIMPLEMENTED) {
			please_git_do_it_for_me();
		} else if (e) {
			libgit_error(e);
		}
	}

	return EXIT_SUCCESS;
}

