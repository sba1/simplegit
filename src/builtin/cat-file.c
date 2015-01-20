#include "cat-file.h"

#include <stdlib.h>
#include <stdio.h>
#include <git2.h>

#include "errors.h"

int cmd_cat_file(struct git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = GIT_OK;
	size_t i, size;
	git_otype type;
	const char *type_string;
	git_oid oid;
	git_odb *odb = NULL;
	git_odb_object *odb_object = NULL;

	if (argc < 3 || strcmp("blob", argv[1]))
	{
		fprintf(stderr, "USAGE: %s blob <objectid>\n", argv[0]);
		goto out;
	}

	if ((err = git_oid_fromstr(&oid, (const char *)argv[argc-1])))
		goto out;

	if ((err = git_repository_odb(&odb, repo)))
		goto out;

	if ((err = git_odb_read(&odb_object, odb, &oid)))
		goto out;

	size = git_odb_object_size(odb_object);
	type = git_odb_object_type(odb_object);
	type_string = git_object_type2string(type);

	for (i=0; i < size; i++)
	{
		printf("%c", ((char*)git_odb_object_data(odb_object))[i]);
	}

out:
	if (err) libgit_error();
	if (odb_object) git_odb_object_free(odb_object);
	if (odb) git_odb_free(odb);
	return rc;
}
