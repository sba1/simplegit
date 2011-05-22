#ifndef CTYPE_H
#define CTYPE_H

#include "git-compat-util.h"

enum {
	S = GIT_SPACE,
	A = GIT_ALPHA,
	D = GIT_DIGIT,
	G = GIT_GLOB_SPECIAL,	/* *, ?, [, \\ */
	R = GIT_REGEX_SPECIAL	/* $, (, ), +, ., ^, {, | */
};

unsigned char sane_ctype[256];

#endif
