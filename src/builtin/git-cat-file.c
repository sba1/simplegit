#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include <string.h>
#include "errors.h"
#include "git-cat-file.h"
#include "git-support.h"
#include "repository.h"
#include "strbuf.h"

int cmd_cat_file(int argc, const char **argv)
{
	if (argc > 3)
		please_git_do_it_for_me();

	git_repository *repo = get_git_repository();
	git_oid oid;
	if (git_oid_mkstr(&oid, (const char *)argv[argc-1]))
		please_git_do_it_for_me();

	git_odb *odb = git_repository_database(repo);
	git_odb_object *odb_object;
	if(git_odb_read(&odb_object, odb, &oid) == GIT_ENOTFOUND)
		libgit_error();
	size_t size = git_odb_object_size(odb_object);
	
	if (argc == 3) {
		if (strcmp(argv[1], "-p") == 0)
		{
			printf("t es là\n");
			for(int i=0; i < (int)size; i++)
				printf("%c", *((char *)git_odb_object_data(odb_object)+i));
		}

		if (strcmp(argv[1], "-t") == 0) {
			printf("t es là\n");
			git_otype type = git_odb_object_type(odb_object);
			printf("%s\n",git_object_type2string(type));
		}

		if (strcmp(argv[1], "-s") == 0) {
			printf("t es là\n");
			printf("%zu\n",size);
		}

		if (strcmp(argv[1], "-e") == 0) {
			printf("t es là\n");
			if(git_odb_exists(odb, &oid) == 1)
				return 0;
		}

		if ((strcmp(argv[1], "blob") == 0) | (strcmp(argv[1], "commit") == 0) | (strcmp(argv[1], "tree") == 0))
		{
			for(int i=0; i < (int)size; i++)
				printf("%c", *((char *)git_odb_object_data(odb_object)+i));
		}
	}
	if (argc == 2) {
		if (strcmp(argv[1], "--batch") == 0) {
			struct strbuf buf = STRBUF_INIT;
			while (strbuf_getline(&buf, stdin, '\n') != EOF) {
				git_oid id;
				if (git_oid_mkstr(&id, buf.buf))
					please_git_do_it_for_me();
				
				git_odb_object *odb_object;
				if(git_odb_read(&odb_object, odb, &oid) == GIT_ENOTFOUND)
					libgit_error();
				size_t size = git_odb_object_size(odb_object);
				git_otype type = git_odb_object_type(odb_object);
				
				printf("bla bla %s %s %zu\n",buf.buf, git_object_type2string(type), size);
				for(int i=0; i < (int)size; i++)
					printf("%c", *((char *)git_odb_object_data(odb_object)+i));
			
			}
		}
	}
	
// 	git_odb_close(odb);
// 	git_odb_object_close(odb_object);
	return EXIT_SUCCESS;
}
