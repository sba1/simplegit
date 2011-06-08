#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include "errors.h"
#include "git-cat-file.h"
#include "git-support.h"
#include "repository.h"
#include "strbuf.h"
#include "parse-options.h"

#define BATCH 1
#define BATCH_CHECK 2

int cmd_cat_file(int argc, const char **argv)
{
	/* Uncomment when it passes the tests */
	please_git_do_it_for_me();

	char opt;
	if (argc != 2 && argc !=3)
		please_git_do_it_for_me();
	if (argc == 3)
		if ((strcmp(argv[1], "blob") == 0) | (strcmp(argv[1], "tree") == 0) | (strcmp(argv[1], "commit") == 0) | (strcmp(argv[1], "tag") == 0 ))
			opt = '0';
		else
			opt = argv[1][1];
	else if (argc == 2 && strcmp(argv[1], "--batch"))
		opt = 'b';
	
	printf("%c\n",opt);
	git_repository *repo = get_git_repository();
	
	git_oid oid;
	if (git_oid_mkstr(&oid, (const char *)argv[argc-1]))
		please_git_do_it_for_me();

	git_odb *odb = git_repository_database(repo);
	git_odb_object *odb_object;
	if(git_odb_read(&odb_object, odb, &oid) == GIT_ENOTFOUND)
		libgit_error();
	
	size_t size = git_odb_object_size(odb_object);
	git_otype type = git_odb_object_type(odb_object);

	const char *type_string = git_object_type2string(type);
	
	switch (opt) {
		case 'p':
			for(int i=0; i < (int)size; i++)
				printf("%c", *((char *)git_odb_object_data(odb_object)+i));
			break;
		case 't':
			printf("%s\n",type_string);
			break;
		case 's':
			printf("%zu\n",size);
			break;
		case 'e':
			if(git_odb_exists(odb, &oid) == 1)
				return 0;
			break;
		case '0' :
			if (strcmp(type_string, argv[1]) == 0)
				for (int i=0; i < (int)size; i++)
					printf("%c", *((char *)git_odb_object_data(odb_object)+i));
 			else
 				please_git_do_it_for_me();
			break;
	}
	
 	//git_odb_close(odb);
 	git_odb_object_close(odb_object);
	return EXIT_SUCCESS;
}
