#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "git-rev-list.h"
#include "repository.h"
#include "git-support.h"
#include "errors.h"
#include "parse-options.h"
#include "strbuf.h"

git_commit* commit = NULL;
git_revwalk *walk = NULL;

static void cleanup() {
	git_commit_close(commit);
	git_revwalk_free(walk);
}

int cmd_rev_list(int argc, const char **argv)
{
	/* Doesn't pass the tests due to bug in libgit2 apparently */
	please_git_do_it_for_me();

	const char *commit_string = NULL;
	const char *format = NULL;
	int i = 0;
	git_repository *repository=NULL;
	git_oid commit_oid;
	git_oid *current_oid;
	char commit_oid_string[GIT_OID_HEXSZ+1];
	size_t len;
	int e;

	/* For now, we only implement --format=oneline */
	if (argc != 3) {
		please_git_do_it_for_me();
	}

	for (i = 1; i < 3; ++i) {
		if (*argv[i] == '-') {
			if (!prefixcmp(argv[i], "--pretty=")) {
				format = argv[i] + strlen("--pretty=");
			}
		} else {
			commit_string = argv[i];
		}
	}

	if (!commit_string) {
		/* Show usage : ask git for now */
		please_git_do_it_for_me();
	}
	if (!format) {
		/* No option or not handled option */
		please_git_do_it_for_me();
	}
	if (strcmp(format, "oneline")) {
		/* Format not supported for now */
		please_git_do_it_for_me();
	}

	/* Supported object specifications are full or short oid */
	/* Create the partial oid (filled with '0's) from the given argument */
	len = strlen(commit_string);
	if (len > GIT_OID_HEXSZ) {
		/* It's not a sha1 */
		please_git_do_it_for_me();
	}
	memcpy(commit_oid_string, commit_string, len * sizeof(char));
	memset(commit_oid_string + len, '0', (GIT_OID_HEXSZ - len) * sizeof(char));
	commit_oid_string[GIT_OID_HEXSZ] = '\0';
	e = git_oid_fromstr(&commit_oid, commit_oid_string);
	if (e) {
		/* Not an OID. The object can be specified in a lot
		 * of different ways (not supported by libgit2 yet).
		 * Fall back to git.
		 */
		please_git_do_it_for_me();
	}

	repository = get_git_repository();

	/* Lookup the commit object */
	e = git_object_lookup_prefix((git_object **)&commit, repository, &commit_oid, len, GIT_OBJ_ANY);
	if (e != GIT_SUCCESS) {
		if (e == GIT_ENOTFOUND) {
			/* Maybe the given argument is not a sha1
			 * but a tag name (which looks like a sha1)
			 * Fall back to git.
			 */
			 please_git_do_it_for_me();
		} else if (e == GIT_EAMBIGUOUSOIDPREFIX) {
			error("%s is an ambiguous prefix", commit_string);
		} else {
			libgit_error();
		}
	}
	if (git_object_type((git_object *)commit) != GIT_OBJ_COMMIT) {
		/* Not a commit : nothing to do */
		cleanup();

		return EXIT_SUCCESS;
	}

	if (git_revwalk_new(&walk, repository)) {
		cleanup();
		libgit_error();
	}

	git_revwalk_sorting(walk, GIT_SORT_TIME);

	current_oid = (git_oid *)git_commit_id(commit);
	if (git_revwalk_push(walk, current_oid)) {
		cleanup();
		libgit_error();
	}

	git_commit_close(commit);
	if ((git_revwalk_next(current_oid, walk)) == GIT_SUCCESS) {
		char oid_string[GIT_OID_HEXSZ+1];
		oid_string[GIT_OID_HEXSZ] = '\0';
		const char *cmsg;

		while (1) {
			git_oid_fmt(oid_string, current_oid);
			if (git_commit_lookup(&commit, repository, current_oid)) {
				libgit_error();
			}

			cmsg  = git_commit_message_short(commit);
			
			git_oid_fmt(oid_string, git_commit_id(commit));
			printf("%s %s\n", oid_string, cmsg);

			if ((git_revwalk_next(current_oid, walk)) != GIT_SUCCESS)
				break;
			git_commit_close(commit);
		}
	}
	
	cleanup();

	return EXIT_SUCCESS;
}
