#include "git-log.h"
#include "git-support.h"

int cmd_log(int argc, const char **argv){
	//TO-DO !
	(void)argc;
	(void)argv;

	please_git_do_it_for_me();
}

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <time.h>
//#include <git2.h>
//#include "errors.h"
//
//int main(int argc, char *argv[]) {
//	if(argc != 2) {
//		printf("Path unspecified !\n");
//		return EXIT_FAILURE;
//	}
//	
//	int e;
//	git_repository *repo;
//	
//	e = git_repository_open(&repo, argv[argc-1]);
//	if (e) error(e);
//	
//	git_revwalk *walk;
//	git_commit *wcommit;
//	const git_oid *oid;
//	
//	git_reference *symbolic_ref;
//	e = git_reference_lookup(&symbolic_ref, repo, "HEAD");
//	if (e) error(e);
//
//	git_reference *direct_ref;
//	e = git_reference_resolve(&direct_ref, symbolic_ref);
//	if (e) error(e);
//
//	oid = git_reference_oid(direct_ref);
//	if (oid == NULL) {
//		printf("Internal error: reference is not direct\n");
//		return EXIT_FAILURE;
//	}
//
//	
//	e = git_revwalk_new(&walk, repo);
//	if (e) error(e);
//
//	git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL);
//
//	e = git_revwalk_push(walk, oid);
//	if (e) error(e);
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
//			e = git_commit_lookup(&wcommit, repo, oid);
//			if (e) error(e);
//
//			cmsg  = git_commit_message_short(wcommit);
//			msg  = git_commit_message(wcommit);
//			cauth = git_commit_author(wcommit);
//			ccommiter = git_commit_committer(wcommit); 
//			time = (time_t)git_commit_time (wcommit);
//			time_string = ctime(&time);
//			time_string[strlen(time_string) - 1] = 0;
//			time_off = git_commit_time_offset(wcommit);
//			
//			git_oid_fmt(oid_string, oid);
//			printf("commit %s\n", oid_string);	
//			printf("Author: %s <%s>\n", cauth->name, cauth->email);
//			/* printf("Commiter: %s <%s>\n", ccommiter->name, ccommiter->email); */
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
//	/* do stuff with the repository */
//	git_repository_free(repo);
//	
//	return EXIT_SUCCESS;
//}
