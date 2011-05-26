#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include "errors.h"
#include "git-mktag.h"
#include "git-support.h"
#include "repository.h"

int cmd_mktag(int argc, const char **argv)
{
	(void)argc;
	(void)argv;
// 	int e;
// 	git_oid oid_tag;
// 	git_repository *repo = get_git_repository();
// 	const char *tag_name = malloc(20); /* ça reste juste une estimation */
// 	git_otype target_type;
// 	const git_oid *target;
// 	const git_signature *tagger;
// 	
// 	const char *message;
// 	
// 	const char *type = malloc(10);
// 	
// 	
// 	scanf("object %s\ntype %s\ntag %s\ntagger %s <%s> %lld %u",
// 	      target->id, type, tag_name, tagger->name, tagger->email, tagger->when->time, tag_name->when->offset);
// 
// 	target_type = git_object_string2type(type);
// 
// 	e = git_tag_create(&oid_tag, repo, tag_name, target, target_type, tagger, message);
// 	if(e) libgit_error();
// 
// 	char *oid_tag_string;
// 	git_oid_fmt(oid_tag_string, &oid_tag);
// 	printf("%s", oid_tag_string);
	return 0;
}

