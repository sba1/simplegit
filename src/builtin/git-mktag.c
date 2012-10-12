#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include <git2.h>
#include "git-mktag.h"
#include "git-support.h"
#include "repository.h"
#include "strbuf.h"



int cmd_mktag(int argc, const char **argv)
{
	please_git_do_it_for_me();
	
	(void)argc;
	(void)argv;
	int e;
	
	if (argc != 1) {
		printf("git mktag < signaturefile\n");
		return 1;
	}

	struct strbuf buf = STRBUF_INIT;
	if (strbuf_read(&buf, 0, 4096) < 0) {
		die_errno("could not read from stdin");
	}
	git_oid oid_tag;
	git_repository *repo = get_git_repository();
	
	e = git_tag_create_frombuffer(&oid_tag,repo,buf.buf,0);
	if( e != GIT_EEXISTS && e != GIT_OK )
		libgit_error();
	
	char *oid_tag_string = malloc(sizeof(char)*41);
	git_oid_fmt(oid_tag_string, &oid_tag);
	oid_tag_string[40] = '\0';
	printf("%s\n", oid_tag_string);
	
	
	return EXIT_SUCCESS;
}

