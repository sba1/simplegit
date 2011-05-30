#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include "errors.h"
#include "git-mktag.h"
#include "git-support.h"
#include "repository.h"
#include "strbuf.h"


/* Global variables that will be fill by parse_stdin() */
char *type;
char *sha1;
char *tag;
char *mail;
char *name;
char *message;
int timestamp;
int timezone_;

int parse_stdin(struct strbuf * buf) {
	char *buffer;
	int typelen;
	char *timestamp_s;
	char *object, *type_line, *tag_line, *tagger_line, *lb, *rb;
	size_t len;
	
	if (strbuf_read(buf, 0, 4096) < 0) {
		die_errno("could not read from stdin");
	}
	buffer = buf->buf;
	
	/* 84 is the minimum size of a tag file */
	if (buf->len < 84)
		return error("wanna fool me ? you obviously got the size wrong !");
	
	buffer[buf->len] = '\0';
	
	/* Verify object line */
	object = buffer;
	if (memcmp(object, "object ", 7))
		return error("char%d: does not start with \"object \"", 0);
	
	if (*(object + 48) == '\n')
		return error("char%d: could not find \"\\n\"", 47);
	
	/* Get the object (sha1) */
	sha1 = object + 7;
	sha1[40] = '\0';
	
// 	printf("sha1 : %s\n", sha1);
	
	
	/* Verify type line */
	type_line = object + 48;
	if (memcmp(type_line, "type ", 5))
		return error("char%d: could not find \"type \"", 48);
	
	/* Verify tag-line */
	tag_line = strchr(type_line, '\n');
	if (!tag_line)
		return error("char%"PRIuMAX": could not find next \"\\n\"",
						 (uintmax_t) (type_line - buffer));
		tag_line++;
	if (memcmp(tag_line, "tag ", 4) || tag_line[4] == '\n')
		return error("char%"PRIuMAX": no \"tag \" found",
						 (uintmax_t) (tag_line - buffer));
		
	/* Get the actual type */
	typelen = tag_line - type_line - strlen("type \n");
	if (typelen >= 0 && (size_t)typelen >= sizeof(type))
		return error("char%"PRIuMAX": type too long",
						 (uintmax_t) (type_line+5 - buffer));
	
	type = type_line+5;
	type[typelen] = '\0';
	
// 	printf("type : %s\n", type);
	
	
	/* TODO:Verify that the object matches */
	// 	if (verify_object(sha1, type))
	// 		return error("char%d: could not verify object %s", 7, sha1_to_hex(sha1));
	
	/* Verify the tag-name: we don't allow control characters or spaces in it */
	/* and get the tag */
	tag_line += 4;
	tag = tag_line;
	int i =0;
	for (;;) {
		unsigned char c = *tag_line++;
		if (c == '\n')
			break;
		i++;
		if (c > ' ')
			continue;
		return error("char%"PRIuMAX": could not verify tag name",
						 (uintmax_t) (tag_line - buffer));
	}
	tag[i] = '\0';
// 	printf("tag : %s\n", tag);
	
	
	
	/* Verify the tagger line */
	tagger_line = tag_line;
	
	if (memcmp(tagger_line, "tagger ", 7))
		return error("char%"PRIuMAX": could not find \"tagger \"",
			 (uintmax_t) (tagger_line - buffer));
	
	/*
	 * Check for correct form for name and email
	 * i.e. " <" followed by "> " on _this_ line
	 * No angle brackets within the name or email address fields.
	 * No spaces within the email address field.
	 */
	tagger_line += 7;
	if (!(lb = strstr(tagger_line, " <")) || !(rb = strstr(lb+2, "> ")) ||
		strpbrk(tagger_line, "<>\n") != lb+1 ||
		strpbrk(lb+2, "><\n ") != rb)
		return error("char%"PRIuMAX": malformed tagger field",
						 (uintmax_t) (tagger_line - buffer));
	
	
	
	/* Check for author name, at least one character, space is acceptable */
	if (lb == tagger_line)
		return error("char%"PRIuMAX": missing tagger name",
						 (uintmax_t) (tagger_line - buffer));
		
	
	/* Get the mail */
	mail = lb+2;
	mail[(size_t)rb-(size_t)lb-2] = '\0';
	
// 	printf("mail : %s\n", mail);
		
	/* Get the name */
	name = tagger_line;
	name[(size_t)lb-(size_t)tagger_line] = '\0';
	
// 	printf("name : %s\n", name);
	
	
	/* timestamp, 1 or more digits followed by space */
	tagger_line = rb + 2;
	if (!(len = strspn(tagger_line, "0123456789")))
		return error("char%"PRIuMAX": missing tag timestamp",
			(uintmax_t) (tagger_line - buffer));
	
	if (*(tagger_line + len) != ' ')
		return error("char%"PRIuMAX": malformed tag timestamp",
			(uintmax_t) (tagger_line + len - buffer));
	
	timestamp_s = tagger_line;
	timestamp_s[len] = '\0';
	timestamp = atoi( timestamp_s );
// 	printf("timestamp_s : %s\n", timestamp_s);
// 	printf("timestamp : %i\n", timestamp);
	
	
	
	tagger_line += len;
	tagger_line++;
	
	
	/* timezone, 5 digits [+-]hhmm, max. 1400 */
	if (!((tagger_line[0] == '+' || tagger_line[0] == '-') &&
		strspn(tagger_line+1, "0123456789") == 4 &&
		tagger_line[5] == '\n' && (timezone_ = atoi(tagger_line+1)) <= 1400))
		return error("char%"PRIuMAX": malformed tag timezone",
						 (uintmax_t) (tagger_line - buffer));
	
	tagger_line += 6;
	
// 	printf("timezone : %i\n", timezone_);
	
	/* Verify the blank line separating the header from the body */
	if (*tagger_line != '\n')
		return error("char%"PRIuMAX": trailing garbage in tag header",
						 (uintmax_t) (tagger_line - buffer));
		
		
	message = tagger_line +1;
	
// 	printf("message : %s\n", message);
	
	return 0;
}



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
	
	e = git_tag_create_frombuffer(&oid_tag,repo,buf.buf);
	if( e != GIT_EEXISTS && e != 0 ) {
		
		libgit_error();
	}
	
	char *oid_tag_string = malloc(sizeof(char)*41);
	git_oid_fmt(oid_tag_string, &oid_tag);
	oid_tag_string[40] = '\0';
	printf("%s\n", oid_tag_string);
	
	/*int e = parse_stdin(&buf);
	if( e != 0 )
		return e;
	
	printf("sha1 : %s\n", sha1);
	printf("type : %s\n", type);
	printf("tag : %s\n", tag);
	printf("mail : %s\n", mail);
	printf("name : %s\n", name);
	printf("timestamp : %i\n", timestamp);
	printf("timezone : %i\n", timezone_);
	printf("message : %s\n", message);
	
	
	e = git_tag_create(&oid_tag, repo, tag_name, target, target_type, tagger, message);
	
	if(e)
		libgit_error();
	*/
	
	return EXIT_SUCCESS;
}

