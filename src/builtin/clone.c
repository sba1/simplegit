#include "clone.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <git2.h>

#include "common.h"
#include "errors.h"

/* Shamelessly borrowed from http://stackoverflow.com/questions/3417837/ */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

typedef struct progress_data {
	git_transfer_progress fetch_progress;
	size_t completed_steps;
	size_t total_steps;
	const char *path;
} progress_data;

static void print_progress(const progress_data *pd)
{
	int network_percent = (100*pd->fetch_progress.received_objects) / pd->fetch_progress.total_objects;
	int index_percent = (100*pd->fetch_progress.indexed_objects) / pd->fetch_progress.total_objects;
	int checkout_percent = pd->total_steps > 0
		? (100 * pd->completed_steps) / pd->total_steps
		: 0.f;
	int kbytes = pd->fetch_progress.received_bytes / 1024;

	fprintf(stderr, "net %3d%% (%4d kb, %5d/%5d)  /  idx %3d%% (%5d/%5d)  /  chk %3d%% (%4" PRIuZ "/%4" PRIuZ ")\r",
		   network_percent, kbytes,
		   pd->fetch_progress.received_objects, pd->fetch_progress.total_objects,
		   index_percent, pd->fetch_progress.indexed_objects, pd->fetch_progress.total_objects,
		   checkout_percent,
		   pd->completed_steps, pd->total_steps);
}

static int fetch_progress(const git_transfer_progress *stats, void *payload)
{
	progress_data *pd = (progress_data*)payload;
	pd->fetch_progress = *stats;
	print_progress(pd);
	return 0;
}
static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
	progress_data *pd = (progress_data*)payload;
	pd->completed_steps = cur;
	pd->total_steps = tot;
	pd->path = path;
	print_progress(pd);
}

int do_clone(git_repository *repo, int argc, char **argv)
{
	int err = 0;
	int rc = EXIT_FAILURE;

	progress_data pd = {{0}};
	git_repository *cloned_repo = NULL;
	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
	git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
	const char *url = argv[1];
	const char *path = argv[2];

	(void)repo; // unused

	// Validate args
	if (argc < 3) {
		fprintf (stderr, "USAGE: %s <url> <path>\n", argv[0]);
		return -1;
	}

	// Set up options
	checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
	checkout_opts.progress_cb = checkout_progress;
	checkout_opts.progress_payload = &pd;
	/* Do not bypass git-aware transport as otherwise, non-local URLs, e.g.
	 * beginning with https://, will be accessed as local files which on AmigaOS
	 * will provoke an Requester.
	 */
	clone_opts.local = GIT_CLONE_NO_LOCAL;
	clone_opts.checkout_opts = checkout_opts;
	clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
	clone_opts.fetch_opts.callbacks.credentials = &cred_acquire_cb;
	clone_opts.fetch_opts.callbacks.payload = &pd;
	clone_opts.fetch_opts.callbacks.certificate_check = certificate_check;

	// Do the clone
	err = git_clone(&cloned_repo, url, path, &clone_opts);
	printf("\n");
	if (err)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (cloned_repo) git_repository_free(cloned_repo);
	return rc;
}
