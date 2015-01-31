#include "mktag.h"

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include <git2.h>
#include "git-support.h"
#include "strbuf.h"

int cmd_mktag(git_repository *repo, int argc,  char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	char sha1buf[GIT_OID_HEXSZ + 1];
	struct strbuf buf = STRBUF_INIT;
	git_oid oid;

	if (argc != 1)
	{
		fprintf (stderr, "USAGE: %s < signaturefile\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (strbuf_read(&buf, 0, 4096) < 0)
	{
		fprintf(stderr, "Couldn't read signature file from stdin.");
		goto out;
	}

	if ((err = git_tag_create_frombuffer(&oid,repo,buf.buf,0)) < 0)
		goto out;

	printf("%s\n", git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &oid));
	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}

