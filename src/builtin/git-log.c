#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "git-log.h"
#include "repository.h"
#include "git-support.h"
#include "errors.h"

int cmd_log(int argc, const char **argv){
	(void)argc;
	(void)argv;

	please_git_do_it_for_me();
	//our implementation is really too minimal
	//to be compared to the git output

//	int e;
//	git_repository *repository = get_git_repository();
//	
//	git_revwalk *walk;
//	git_commit *wcommit;
//	const git_oid *oid;
//	
//	git_reference *symbolic_ref;
//	e = git_reference_lookup(&symbolic_ref, repository, "HEAD");
//	if (e) libgit_error(e);
//
//	git_reference *direct_ref;
//	e = git_reference_resolve(&direct_ref, symbolic_ref);
//	if (e) libgit_error(e);
//
//	oid = git_reference_oid(direct_ref);
//	if (oid == NULL) {
//		die("Internal error: reference is not direct\n");
//	}
//
//	
//	e = git_revwalk_new(&walk, repository);
//	if (e) libgit_error(e);
//
//	git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL);
//
//	e = git_revwalk_push(walk, oid);
//	if (e) libgit_error(e);
//
//	const git_signature *cauth, *ccommiter;
//	const char *cmsg,*msg;
//	time_t time;
//	int time_off;
//	
//	if ((git_revwalk_next((git_oid *)oid, walk)) == GIT_SUCCESS) {
//		char oid_string[GIT_OID_HEXSZ+1];
//		oid_string[GIT_OID_HEXSZ] = 0;
//		char *time_string;
//		while(1) {
//			e = git_commit_lookup(&wcommit, repository, oid);
//			if (e) libgit_error(e);
//
//			cmsg  = git_commit_message_short(wcommit);
//			msg  = git_commit_message(wcommit);
//			cauth = git_commit_author(wcommit);
//			ccommiter = git_commit_committer(wcommit);
//			(void)ccommiter;
//			time = (time_t)git_commit_time (wcommit);
//			time_string = ctime(&time);
//			time_string[strlen(time_string) - 1] = 0;
//			time_off = git_commit_time_offset(wcommit);
//			
//			git_oid_fmt(oid_string, oid);
//			printf("commit %s\n", oid_string);
//			printf("Author: %s <%s>\n", cauth->name, cauth->email);
//
//			printf("Date:   %s %u\n\n", time_string, time_off);
//			printf("%s %s", cmsg, msg);
//			if ((git_revwalk_next((git_oid *)oid, walk)) != GIT_SUCCESS)
//				break;
//			printf("\n");
//		}
//	}
//	
//
//	git_revwalk_free(walk);
//	
//	return EXIT_SUCCESS;
}
