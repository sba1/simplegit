#include "diff.h"

#include <stdio.h>
#include <git2.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

static int resolve_to_tree(	git_repository *repo, const char *identifier, git_tree **tree)
{
	int err = 0;
	size_t len = strlen(identifier);
	git_oid oid;
	git_object *obj = NULL;

	/* try to resolve as OID */
	if (git_oid_fromstrn(&oid, identifier, len) == 0)
		git_object_lookup_prefix(&obj, repo, &oid, len, GIT_OBJ_ANY);

	/* try to resolve as reference */
	if (obj == NULL)
	{
		git_reference *ref, *resolved;
		if (git_reference_lookup(&ref, repo, identifier) == 0)
		{
			git_reference_resolve(&resolved, ref);
			git_reference_free(ref);
			if (resolved)
			{
				git_object_lookup(&obj, repo, git_reference_target(resolved), GIT_OBJ_ANY);
				git_reference_free(resolved);
			}
		}
	}

	if (obj == NULL)
		return GIT_ENOTFOUND;

	switch (git_object_type(obj)) {
	case GIT_OBJ_TREE:
		*tree = (git_tree *)obj;
		break;
	case GIT_OBJ_COMMIT:
		err = git_commit_tree(tree, (git_commit *)obj);
		git_object_free(obj);
		break;
	default:
		err = GIT_ENOTFOUND;
	}

	return err;
}

char *colors[] = {
	"\033[m", /* reset */
	"\033[1m", /* bold */
	"\033[31m", /* red */
	"\033[32m", /* green */
	"\033[36m" /* cyan */
};

static int printer(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	void *data)
{
	int *last_color = data, color = 0;

	(void)delta; (void)hunk;

	if (*last_color >= 0) {
		switch (line->origin) {
		case GIT_DIFF_LINE_ADDITION: color = 3; break;
		case GIT_DIFF_LINE_DELETION: color = 2; break;
		case GIT_DIFF_LINE_ADD_EOFNL: color = 3; break;
		case GIT_DIFF_LINE_DEL_EOFNL: color = 2; break;
		case GIT_DIFF_LINE_FILE_HDR: color = 1; break;
		case GIT_DIFF_LINE_HUNK_HDR: color = 4; break;
		default: color = 0;
		}
		if (color != *last_color) {
			if (*last_color == 1 || color == 1)
				fputs(colors[0], stdout);
			fputs(colors[color], stdout);
			*last_color = color;
		}
	}

	fputs(line->content, stdout);
	return 0;
}

static int check_uint16_param(const char *arg, const char *pattern, uint16_t *val)
{
	size_t len = strlen(pattern);
	uint16_t strval;
	char *endptr = NULL;
	if (strncmp(arg, pattern, len))
		return 0;
	strval = strtoul(arg + len, &endptr, 0);
	if (endptr == arg)
		return 0;
	*val = strval;
	return 1;
}

static int check_str_param(const char *arg, const char *pattern, const char **val)
{
	size_t len = strlen(pattern);
	if (strncmp(arg, pattern, len))
		return 0;
	*val = (const char *)(arg + len);
	return 1;
}

int cmd_diff(git_repository *repo, int argc, char **argv)
{
	int err = 0;
	int rc = EXIT_FAILURE;
	git_tree *t1 = NULL, *t2 = NULL;
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	git_diff *diff;
	int i, color = -1, compact = 0, cached = 0;
	char *a, *treeish1 = NULL, *treeish2 = NULL;

	/* parse arguments as copied from git-diff */

	for (i = 1; i < argc; ++i) {
		a = argv[i];

		if (a[0] != '-') {
			if (treeish1 == NULL)
				treeish1 = a;
			else if (treeish2 == NULL)
				treeish2 = a;
			else
			{
				fprintf(stderr,"Only one or two tree identifiers can be provided\n");
				goto out;
			}
		}
		else if (!strcmp(a, "-p") || !strcmp(a, "-u") ||
			!strcmp(a, "--patch"))
			compact = 0;
		else if (!strcmp(a, "--cached"))
			cached = 1;
		else if (!strcmp(a, "--name-status"))
			compact = 1;
		else if (!strcmp(a, "--color"))
			color = 0;
		else if (!strcmp(a, "--no-color"))
			color = -1;
		else if (!strcmp(a, "-R"))
			opts.flags |= GIT_DIFF_REVERSE;
		else if (!strcmp(a, "-a") || !strcmp(a, "--text"))
			opts.flags |= GIT_DIFF_FORCE_TEXT;
		else if (!strcmp(a, "--ignore-space-at-eol"))
			opts.flags |= GIT_DIFF_IGNORE_WHITESPACE_EOL;
		else if (!strcmp(a, "-b") || !strcmp(a, "--ignore-space-change"))
			opts.flags |= GIT_DIFF_IGNORE_WHITESPACE_CHANGE;
		else if (!strcmp(a, "-w") || !strcmp(a, "--ignore-all-space"))
			opts.flags |= GIT_DIFF_IGNORE_WHITESPACE;
		else if (!strcmp(a, "--ignored"))
			opts.flags |= GIT_DIFF_INCLUDE_IGNORED;
		else if (!strcmp(a, "--untracked"))
			opts.flags |= GIT_DIFF_INCLUDE_UNTRACKED;
		else if (!check_uint16_param(a, "-U", &opts.context_lines) &&
			!check_uint16_param(a, "--unified=", &opts.context_lines) &&
			!check_uint16_param(a, "--inter-hunk-context=",
				&opts.interhunk_lines) &&
			!check_str_param(a, "--src-prefix=", &opts.old_prefix) &&
			!check_str_param(a, "--dst-prefix=", &opts.new_prefix))
		{
			fprintf(stderr,"Unknown option %s\n", a);
			goto out;
		}
	}

	if (treeish1)
	{
		if ((err = resolve_to_tree(repo, treeish1, &t1)) != GIT_OK)
			goto out;
	}
	if (treeish2)
	{
		if ((err = resolve_to_tree(repo, treeish2, &t2)) != GIT_OK)
			goto out;
	}

	/* <sha1> <sha2> */
	/* <sha1> --cached */
	/* <sha1> */
	/* --cached */
	/* nothing */

	if (t1 && t2)
	{
		if ((err = git_diff_tree_to_tree(&diff, repo, t1, t2, &opts)) != GIT_OK)
			goto out;
	}
	else if (t1 && cached)
	{
		if ((err = git_diff_tree_to_index(&diff, repo, t1, NULL, &opts) != GIT_OK))
			goto out;
	}
	else if (t1)
	{
		git_diff *diff2;
		if ((err = git_diff_tree_to_index(&diff, repo, t1, NULL, &opts)) != GIT_OK)
			goto out;
		if ((err = git_diff_index_to_workdir(&diff2, repo, NULL, &opts)) != GIT_OK)
			goto out;
		if ((err = git_diff_merge(diff, diff2)) != GIT_OK)
			goto out;
		git_diff_free(diff2);
	}
	else if (cached) {
		if ((err = resolve_to_tree(repo, "HEAD", &t1)) != GIT_OK)
			goto out;
		if ((err = git_diff_tree_to_index(&diff, repo, t1, NULL, &opts)) != GIT_OK)
			goto out;
	}
	else {
		if ((err = git_diff_index_to_workdir(&diff, repo, NULL, &opts)) != GIT_OK)
			goto out;
	}

	if (color >= 0)
		fputs(colors[0], stdout);

	if (compact)
	{
		if ((err = git_diff_print(diff, GIT_DIFF_FORMAT_NAME_STATUS, printer,  &color)) != GIT_OK)
			goto out;
	} else
	{
		if ((err = git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, printer, &color)) != GIT_OK)
			goto out;
	}

	if (color >= 0)
		fputs(colors[0], stdout);

	rc = EXIT_SUCCESS;
out:
	if (err)
	{
		printf("%d\n",err);
		libgit_error();
	}
	if (diff) git_diff_free(diff);
	if (t1) git_tree_free(t1);
	if (t2) git_tree_free(t2);

	return rc;
}
