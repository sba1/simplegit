#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

static int use_unnamed(git_repository *repo, const char *url)
{
	git_remote *remote = NULL;
	int error;
	const git_remote_head **refs;
	size_t refs_len, i;

	// Create an instance of a remote from the URL. The transport to use
	// is detected from the URL
	error = git_remote_create_inmemory(&remote, repo, NULL, url);
	if (error < 0)
		goto cleanup;

	// When connecting, the underlying code needs to know wether we
	// want to push or fetch
	error = git_remote_connect(remote, GIT_DIRECTION_FETCH);
	if (error < 0)
		goto cleanup;

	/**
	 * Get the list of references on the remote and print out
	 * their name next to what they point to.
	 */
	if ((error = git_remote_ls(&refs, &refs_len, remote)) < 0)
		goto cleanup;

	for (i = 0; i < refs_len; i++) {
		char oid[GIT_OID_HEXSZ + 1] = {0};
		git_oid_fmt(oid, &refs[i]->oid);
		printf("%s\t%s\n", oid, refs[i]->name);
	}
cleanup:
	git_remote_free(remote);
	return error;
}

static int use_remote(git_repository *repo, char *name)
{
	git_remote *remote = NULL;
	int error;
	const git_remote_head **refs;
	size_t refs_len, i;

	// Find the remote by name
	error = git_remote_load(&remote, repo, name);
	if (error < 0)
		goto cleanup;

	error = git_remote_connect(remote, GIT_DIRECTION_FETCH);
	if (error < 0)
		goto cleanup;

	/**
	 * Get the list of references on the remote and print out
	 * their name next to what they point to.
	 */
	if ((error = git_remote_ls(&refs, &refs_len, remote)) < 0)
		goto cleanup;

	for (i = 0; i < refs_len; i++) {
		char oid[GIT_OID_HEXSZ + 1] = {0};
		git_oid_fmt(oid, &refs[i]->oid);
		printf("%s\t%s\n", oid, refs[i]->name);
	}

cleanup:
	git_remote_free(remote);
	return error;
}

// This gets called to do the work. The remote can be given either as
// the name of a configured remote or an URL.

int ls_remote(git_repository *repo, int argc, char **argv)
{
	int error;

	argc = argc;
	/* If there's a ':' in the name, assume it's an URL */
	if (strchr(argv[1], ':') != NULL) {
		error = use_unnamed(repo, argv[1]);
	} else {
		error = use_remote(repo, argv[1]);
	}

	return error;
}
