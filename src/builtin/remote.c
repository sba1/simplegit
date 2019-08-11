/**
 * @file
 *
 * Implements the remote subcommand. Based on the corresponding example
 * supplied in libgit2.
 */

#include "common.h"

#include <stdio.h>
#include <string.h>

enum subcmd
{
	subcmd_add,
	subcmd_remove,
	subcmd_rename,
	subcmd_seturl,
	subcmd_show,
};

struct opts {
	enum subcmd cmd;

	/* for command-specific args */
	int argc;
	char **argv;
};

static int cmd_add(git_repository *repo, struct opts *o, int *err)
{
	char *name, *url;
	int rc = EXIT_FAILURE;
	git_remote *remote = {0};

	if (o->argc != 2)
	{
		fprintf(stderr, "you need to specify a name and URL");
		return 1;
	}

	name = o->argv[0];
	url = o->argv[1];

	if ((*err = git_remote_create(&remote, repo, name, url)))
	{
		goto out;
	}
	rc = 0;
out:
	return rc;
}

static int cmd_remove(git_repository *repo, struct opts *o, int *err)
{
	char *name;
	int rc = EXIT_FAILURE;

	if (o->argc != 1)
	{
		fprintf(stderr, "you need to specify a name\n");
		goto out;
	}

	name = o->argv[0];

	if ((*err = git_remote_delete(repo, name)))
	{
		goto out;
	}
	rc = 0;
out:
	return rc;
}

static int cmd_rename(git_repository *repo, struct opts *o, int *err)
{
	int i, retval = EXIT_FAILURE;
	char *old, *new;
	git_strarray problems = {0};

	if (o->argc != 2)
	{
		fprintf(stderr, "you need to specify old and new remote name\n");
		goto out;
	}

	old = o->argv[0];
	new = o->argv[1];

	if ((*err = git_remote_rename(&problems, repo, old, new)))
	{
		goto out;
	}

	for (i = 0; i < (int) problems.count; i++) {
		puts(problems.strings[0]);
	}

	git_strarray_free(&problems);

	retval = 0;
out:
	return retval;
}

static int cmd_seturl(git_repository *repo, struct opts *o, int *err)
{
	int i, retval = EXIT_FAILURE, push = 0;
	char *name = NULL, *url = NULL;

	for (i = 0; i < o->argc; i++) {
		char *arg = o->argv[i];

		if (!strcmp(arg, "--push")) {
			push = 1;
		} else if (arg[0] != '-' && name == NULL) {
			name = arg;
		} else if (arg[0] != '-' && url == NULL) {
			url = arg;
		} else {
			fprintf(stderr, "invalid argument to set-url\n");
			goto out;
		}
	}

	if (name == NULL || url == NULL)
	{
		fprintf(stderr, "you need to specify remote and the new URL\n");
		goto out;
	}

	if (push)
		*err = git_remote_set_pushurl(repo, name, url);
	else
		*err = git_remote_set_url(repo, name, url);

	if ((*err))
	{
		goto out;
	}

	retval = 0;
out:
	return retval;
}

static int cmd_show(git_repository *repo, struct opts *o, int *err)
{
	int i, retval = EXIT_FAILURE;
	const char *arg, *name, *fetch, *push;
	int verbose = 0;
	git_strarray remotes = {0};
	git_remote *remote = {0};

	for (i = 0; i < o->argc; i++) {
		arg = o->argv[i];

		if (!strcmp(arg, "-v") || !strcmp(arg, "--verbose")) {
			verbose = 1;
		}
	}

	if ((*err = git_remote_list(&remotes, repo)))
	{
		goto out;
	}

	for (i = 0; i < (int) remotes.count; i++) {
		name = remotes.strings[i];
		if (!verbose) {
			puts(name);
			continue;
		}

		if ((*err = git_remote_lookup(&remote, repo, name)))
		{
			goto out;
		}

		fetch = git_remote_url(remote);
		if (fetch)
			printf("%s\t%s (fetch)\n", name, fetch);
		push = git_remote_pushurl(remote);
		/* use fetch URL if no distinct push URL has been set */
		push = push ? push : fetch;
		if (push)
			printf("%s\t%s (push)\n", name, push);

		git_remote_free(remote);
	}

	retval = EXIT_SUCCESS;
	git_strarray_free(&remotes);
out:
	return retval;
}

static void parse_subcmd(struct opts *opt, int argc, char **argv)
{
	char *arg = argv[1];
	enum subcmd cmd = -1;

	if (argc < 2)
	{
		cmd = subcmd_show;
		opt->argc = 0;
		opt->argv = NULL;
		goto out;
	}

	if (!strcmp(arg, "add")) {
		cmd = subcmd_add;
	} else if (!strcmp(arg, "remove") || !strcmp(arg, "rm")) {
		cmd = subcmd_remove;
	} else if (!strcmp(arg, "rename")) {
		cmd = subcmd_rename;
	} else if (!strcmp(arg, "set-url")) {
		cmd = subcmd_seturl;
	} else if (!strcmp(arg, "show")) {
		cmd = subcmd_show;
	} else if (!strcmp(arg,"-v")) {
		if (argc > 2) {
			fprintf(stderr, "Only -v is supported!\n");
			goto out;
		}
		opt->argv = &argv[1];
		opt->argc = 1;
		cmd = subcmd_show;
		goto out;
	} else {
		fprintf(stderr, "The sub command %s is not supported!\n", arg);
		goto out;
	}

	opt->argc = argc - 2; /* executable and subcommand are removed */
	opt->argv = argv + 2;
out:
	opt->cmd = cmd;
}

/*****************************************************************************/

int cmd_remote(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	struct opts opt = {0};

	parse_subcmd(&opt, argc, argv);

	switch (opt.cmd)
	{
	case subcmd_add:
		rc = cmd_add(repo, &opt, &err);
		break;
	case subcmd_remove:
		rc = cmd_remove(repo, &opt, &err);
		break;
	case subcmd_rename:
		rc = cmd_rename(repo, &opt, &err);
		break;
	case subcmd_seturl:
		rc = cmd_seturl(repo, &opt, &err);
		break;
	case subcmd_show:
		rc = cmd_show(repo, &opt, &err);
		break;
	}

	return rc;
}
