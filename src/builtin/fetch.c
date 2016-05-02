#include <stdio.h>
#include <stdlib.h>

#include <git2.h>

#include "common.h"
#include "errors.h"

struct dl_data {
	git_remote *remote;
	int ret;
	int finished;
};

static int progress_cb(const char *str, int len, void *data)
{
	(void)data;
	printf("remote: %.*s", len, str);
	fflush(stdout); /* We don't have the \n to force the flush */
	return 0;
}

/**
 * This function gets called for each remote-tracking branch that gets
 * updated. The message we output depends on whether it's a new one or
 * an update.
 */
static int update_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
	char a_str[GIT_OID_HEXSZ+1], b_str[GIT_OID_HEXSZ+1];
	(void)data;

	git_oid_fmt(b_str, b);
	b_str[GIT_OID_HEXSZ] = '\0';

	if (git_oid_iszero(a)) {
		printf("[new]     %.20s %s\n", b_str, refname);
	} else {
		git_oid_fmt(a_str, a);
		a_str[GIT_OID_HEXSZ] = '\0';
		printf("[updated] %.10s..%.10s %s\n", a_str, b_str, refname);
	}

	return 0;
}

/** Entry point for this command */
int fetch(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
	git_fetch_options fetch_options = GIT_FETCH_OPTIONS_INIT;

	const git_transfer_progress *stats;
	git_remote *remote = NULL;

	if (argc < 2) {
		fprintf(stderr, "usage: %s fetch <repo>\n", argv[-1]);
		return EXIT_FAILURE;
	}

	// Figure out whether it's a named remote or a URL
	printf("Fetching %s for repo at %s\n", argv[1], git_repository_path(repo));
	if (git_remote_lookup(&remote, repo, argv[1]) < 0) {
		if ((err = git_remote_create_anonymous(&remote, repo, argv[1])) < 0)
			goto out;
	}

	// Set up the callbacks (only update_tips for now)
	callbacks.update_tips = &update_cb;
	callbacks.sideband_progress = &progress_cb;
	callbacks.credentials = cred_acquire_cb;
	callbacks.certificate_check = certificate_check;

	stats = git_remote_stats(remote);

	if ((err = git_remote_connect(remote, GIT_DIRECTION_FETCH, &callbacks, NULL) < 0))
		goto out;

	fetch_options.callbacks = callbacks;

	// Download the packfile and index it. This function updates the
	// amount of received data and the indexer stats which lets you
	// inform the user about progress.
	if ((err = git_remote_download(remote, NULL, &fetch_options) < 0))
		goto out;

	/**
	 * If there are local objects (we got a thin pack), then tell
	 * the user how many objects we saved from having to cross the
	 * network.
	 */
	if (stats->local_objects > 0) {
		printf("\rReceived %d/%d objects in %zu bytes (used %d local objects)\n",
		       stats->indexed_objects, stats->total_objects, stats->received_bytes, stats->local_objects);
	} else{
		printf("\rReceived %d/%d objects in %zu bytes\n",
			stats->indexed_objects, stats->total_objects, stats->received_bytes);
	}

	// Disconnect the underlying connection to prevent from idling.
	git_remote_disconnect(remote);

	// Update the references in the remote's namespace to point to the
	// right commits. This may be needed even if there was no packfile
	// to download, which can happen e.g. when the branches have been
	// changed but all the needed objects are available locally.
	if ((err = git_remote_update_tips(remote, &callbacks, fetch_options.update_fetchhead, fetch_options.download_tags, NULL)) < 0)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	if (remote) git_remote_free(remote);
	return rc;
}
