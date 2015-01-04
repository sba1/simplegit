#include "cat-file.h"

#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include "errors.h"
#include "git-support.h"
#include "ls-tree.h"
#include "strbuf.h"

int cmd_cat_file(struct git_repository *repo, int argc, char **argv)
{
	/* Uncomment when it passes the tests */
	please_git_do_it_for_me();

	char opt;
	if (argc != 2 && argc !=3)
		please_git_do_it_for_me();

	if (argc == 3){
		if ((strcmp(argv[1], "blob") == 0) || (strcmp(argv[1], "tree") == 0) || (strcmp(argv[1], "commit") == 0) || (strcmp(argv[1], "tag") == 0 ))
			opt = '0';
		else
			opt = argv[1][1];
	}
	else if (argc == 2 && (strcmp(argv[1], "--batch") == 0))
		opt = 'a';
	
	git_oid oid;
	if (git_oid_fromstr(&oid, (const char *)argv[argc-1]))
		please_git_do_it_for_me();

	git_odb *odb;
	git_repository_odb(&odb, repo);
	git_odb_object *odb_object;
	if(git_odb_read(&odb_object, odb, &oid) == GIT_ENOTFOUND)
		libgit_error();

	size_t size = git_odb_object_size(odb_object);
	git_otype type = git_odb_object_type(odb_object);

	const char *type_string = git_object_type2string(type);

	switch (opt) {
		case 'p':
			if (strcmp(type_string, "tree") == 0) {
				printf("vous etes là\n");

 				char ** arg = malloc(2);
 				strcpy(arg[0], "ls-tree");
 				strcpy(arg[1], argv[2]);
 				int e = cmd_ls_tree(repo, 2, arg);
 				if (e == 0 )
 					printf("succes\n");
 				else
 					printf("echec\n");
			}
			else {
				for(int i=0; i < (int)size; i++)
					printf("%c", *((char *)git_odb_object_data(odb_object)+i));
			}
			break;
		case 't':
			printf("%s\n",type_string);
			break;
		case 's':
			printf("%zu\n",size);
			break;
		case 'e':
			if(git_odb_exists(odb, &oid) == 1) {
				return 0;
			}
			break;
		case '0' :
			if (strcmp(type_string, argv[1]) == 0) {
				for (int i=0; i < (int)size; i++)
					printf("%c", *((char *)git_odb_object_data(odb_object)+i));
			}
 			else
 				please_git_do_it_for_me();
			break;
		case 'a' :
			please_git_do_it_for_me();
// 			if (strbuf_read(&buf, 0, 4096) < 0)
// 				die_errno("could not read from stdin");
// 
// 			git_oid id;
// 			if (git_oid_mkstr(&id, buf.buf))
// 				please_git_do_it_for_me();
// 
// 			git_odb_object *odb_object1;
// 			if(git_odb_read(&odb_object1, odb, &id) == GIT_ENOTFOUND)
// 				libgit_error();
// 			size_t size1 = git_odb_object_size(odb_object1);
// 			git_otype type1 = git_odb_object_type(odb_object1);
// 
// 			printf("bla bla %s %s %zu\n",buf.buf, git_object_type2string(type1), size1);
// 			for(int i=0; i < (int)size1; i++)
// 				printf("%c", *((char *)git_odb_object_data(odb_object1)+i));
// 			fflush(stdout);
			break;
	}
	//git_odb_object_close(odb_object);
	return EXIT_SUCCESS;
}
