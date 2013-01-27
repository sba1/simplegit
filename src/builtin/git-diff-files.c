#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-diff-files.h"
#include "git-support.h"
#include "quote.h"

static int file_cb(const git_diff_delta *delta, float progress, void *payload)
{
	char ooid_str[GIT_OID_HEXSZ+1];
	char noid_str[GIT_OID_HEXSZ+1];
	char status;

	progress = progress;
	payload = payload;

	git_oid_tostr(noid_str,sizeof(noid_str),&delta->new_file.oid);
	git_oid_tostr(ooid_str,sizeof(ooid_str),&delta->old_file.oid);
	switch (delta->status)
	{
		case	GIT_DELTA_MODIFIED: status = 'M'; break;
		case	GIT_DELTA_DELETED: status = 'D'; break;
		default: status = '?'; break;
	}
	printf(":%06o %06o %s %s %c\t%s\n",delta->new_file.mode,delta->old_file.mode,noid_str,ooid_str,status,delta->new_file.path);
	return 0;
}

int cmd_diff_files(git_repository *repo, int argc, char **argv)
{
	int i;
	int err = 0;
	int rc = EXIT_FAILURE;
	int filec = 0;
	char **filev = NULL;

	size_t index_entries;

	git_index *idx = NULL;

	git_diff_delta delta;

	for (i=1;i<argc;i++)
	{
/*		if (!strcmp("-r",argv[i])) opts.recurse = 1;
		else if (!strcmp("-t",argv[i])) opts.show_tree_entries = 1;
		else */if (argv[i][0] != '-')
		{
			filev = &argv[i];
			filec = argc - i;
			break;
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	memset(&delta,0,sizeof(delta));
	index_entries = git_index_entrycount(idx);

	for (i=0;i<(int)index_entries;i++)
	{
		const git_index_entry *e;
		struct stat st;

		e = git_index_get_byindex(idx,i);

		delta.new_file.path = e->path;
		delta.new_file.oid = e->oid;
		delta.new_file.mode = e->mode;

		if (!lstat(e->path,&st))
		{
			delta.old_file.path = e->path;
			delta.old_file.mode = sgit_get_mode(st.st_mode);
			git_oid_fromstrn(&delta.old_file.oid,"",0);
			delta.status = GIT_DELTA_MODIFIED;
		} else
		{
			delta.status = GIT_DELTA_DELETED;
			delta.old_file.mode = 0;
		}

		file_cb(&delta,0,NULL);
	}

	rc = EXIT_SUCCESS;
out:
	if (err)
	{
		printf("%d\n",err);
		libgit_error();
	}
	if (idx) git_index_free(idx);
	return rc;
}
