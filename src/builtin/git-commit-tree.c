#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include <git2.h>
#include "git.h"
#include "git-commit-tree.h"
#include "git-support.h"
#include "repository.h"
#include "date.h"
#include "strbuf.h"
#include "environment.h"

git_signature *author_signature = NULL;
git_signature *committer_signature = NULL;
git_tree *tree = NULL;
struct strbuf buffer = STRBUF_INIT;
int parent_count = 0;
git_commit **parents = NULL;

static void cleanup() {
	int i;
	git_tree_close(tree);
	strbuf_release(&buffer);
	git_signature_free(author_signature);
	git_signature_free(committer_signature);

	if (parents) {
		for (i = 0; i < parent_count; ++i) {
			if (parents[i]) {
				git_commit_close(parents[i]);
			}
		}
		free(parents);
	}
}

int cmd_commit_tree(int argc, const char **argv)
{
	char *author_name = NULL;
	char *author_email = NULL;
	char *author_date = NULL;
	char *committer_name = NULL;
	char *committer_email = NULL;
	char *committer_date = NULL;
	unsigned long author_timestamp;
	int author_offset;
	unsigned long committer_timestamp;
	int committer_offset;
	int e;
	git_repository *repo = NULL;
	git_oid tree_oid;
	char commit_oid_string[GIT_OID_HEXSZ+1];
	char tree_oid_string[GIT_OID_HEXSZ+1];
	git_oid commit_oid;
	size_t len;
	int i;

	if (argc < 2 || !strcmp(argv[1], "-h")) {
		/* Show usage */
		please_git_do_it_for_me();
	}

	if (argc > 2 && argc%2 != 0) {
		/* If parents are specified they are preceded by '-p'
		 * => even number of arguments */
		please_git_do_it_for_me();
	}

	author_name = getenv(GIT_AUTHOR_NAME_ENVIRONMENT);
	author_email = getenv(GIT_AUTHOR_EMAIL_ENVIRONMENT);
	author_date = getenv(GIT_AUTHOR_DATE_ENVIRONMENT);
	committer_name = getenv(GIT_COMMITTER_NAME_ENVIRONMENT);
	committer_email = getenv(GIT_COMMITTER_EMAIL_ENVIRONMENT);
	committer_date = getenv(GIT_COMMITTER_DATE_ENVIRONMENT);
	
	if (!author_name || !author_email || !author_date ||
		!committer_name || !committer_email || !committer_date) {
		/* Read from config file. Ask git for help for now */
		please_git_do_it_for_me();
	}

	e = parse_date_basic(author_date, &author_timestamp, &author_offset);
	if (e) {
		/* Bad date format. Git will apparently use system time */
		please_git_do_it_for_me();
	}

	e = parse_date_basic(committer_date, &committer_timestamp, &committer_offset);
	if (e) {
		please_git_do_it_for_me();
	}

	/* Supported object specifications are full or short oid */
	/* Create the partial oid (filled with '0's) from the given argument */
	len = strlen(argv[1]);
	if (len > GIT_OID_HEXSZ) {
		/* It's not a sha1 */
		please_git_do_it_for_me();
	}
	memcpy(tree_oid_string, argv[1], len * sizeof(char));
	memset(tree_oid_string + len, '0', (GIT_OID_HEXSZ - len) * sizeof(char));
	tree_oid_string[GIT_OID_HEXSZ] = '\0';
	e = git_oid_fromstr(&tree_oid, tree_oid_string);
	if (e) {
		/* Not an OID. The object can be specified in a lot
		 * of different ways (not supported by libgit2 yet).
		 * Fall back to git.
		 */
		please_git_do_it_for_me();
	}

	repo = get_git_repository();
	/* Lookup the tree object */
	e = git_object_lookup_prefix((git_object **)&tree, repo, &tree_oid, len, GIT_OBJ_ANY);
	if (e != GIT_SUCCESS) {
		if (e == GIT_ENOTFOUND) {
			/* Maybe the given argument is not a sha1
			 * but a tag name (which looks like a sha1)
			 * Fall back to git.
			 */
			 please_git_do_it_for_me();
		} else if (e == GIT_EAMBIGUOUSOIDPREFIX) {
			error("%s is an ambiguous prefix", argv[1]);
		} else {
			libgit_error();
		}
	}
	if (git_object_type((git_object *)tree) != GIT_OBJ_TREE) {
		if (len < GIT_OID_HEXSZ) {
			/* Get the full oid, in case the given argument was a short oid */
			git_oid_fmt(tree_oid_string, git_tree_id(tree));
		}

		cleanup();
		error("%s is not a valid 'tree' object", tree_oid_string);
	}

	/* Get parent commits */
	parent_count = argc / 2 - 1;
	parents = (git_commit **)malloc(parent_count * sizeof(git_commit *));
	for (i = 0; i < parent_count; ++i)
		parents[i] = NULL;

	for (i = 2; i < argc; i += 2) {
		char parent_oid_string[GIT_OID_HEXSZ+1];
		git_oid parent_oid;
		git_commit **parent_commit;

		if (strcmp(argv[i], "-p")) {
			/* Bad use : show usage */
			please_git_do_it_for_me();
		}

		len = strlen(argv[i+1]);
		if (len > GIT_OID_HEXSZ) {
			/* It's not a sha1 */
			please_git_do_it_for_me();
		}
		memcpy(parent_oid_string, argv[i+1], len * sizeof(char));
		memset(parent_oid_string + len, '0', (GIT_OID_HEXSZ - len) * sizeof(char));
		parent_oid_string[GIT_OID_HEXSZ] = '\0';
		e = git_oid_fromstr(&parent_oid, parent_oid_string);
		if (e) {
			/* Not an oid. Maybe parent is specified in another way */
			please_git_do_it_for_me();
		}
		parent_commit = &parents[i/2 - 1];
		e = git_object_lookup_prefix((git_object **)parent_commit, repo, &parent_oid, len, GIT_OBJ_ANY);
		if (e != GIT_SUCCESS) {
			if (e == GIT_ENOTFOUND) {
				 please_git_do_it_for_me();
			} else if (e == GIT_EAMBIGUOUSOIDPREFIX) {
				error("%s is an ambiguous prefix", argv[i+1]);
			} else {
				cleanup();
				libgit_error();
			}
		}
		if (git_object_type((git_object *)*parent_commit) != GIT_OBJ_COMMIT) {
			if (len < GIT_OID_HEXSZ) {
				/* Get the full oid, in case the given argument was a short oid */
				git_oid_fmt(parent_oid_string, git_commit_id(*parent_commit));
			}

			cleanup();
			error("%s is not a valid 'commit' object", parent_oid_string);
		}
	}

	if (git_signature_new(&author_signature, author_name, author_email, author_timestamp, author_offset) != 0) {
		cleanup();
		libgit_error();
	}

	if (git_signature_new(&committer_signature, committer_name, committer_email, committer_timestamp, committer_offset) != 0) {
		cleanup();
		libgit_error();
	}

	if (strbuf_read(&buffer, 0, 0) < 0) {
		cleanup();
		error("Failed to read changelog");
	}
	
	/* was _o  template slightly different, might need some fixing */
	
	e = git_commit_create(&commit_oid, repo, NULL, author_signature, committer_signature,
				NULL, buffer.buf, tree, parent_count, (const git_commit **)parents);
	if (e) {
		cleanup();
		libgit_error();
	}

	/* Print to stdout */
	git_oid_fmt(commit_oid_string, &commit_oid);
	commit_oid_string[GIT_OID_HEXSZ] = '\0';
	printf("%s\n", commit_oid_string);
	
	cleanup();

	return EXIT_SUCCESS;
}
