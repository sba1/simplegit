#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "print.h"


struct optarg
{
	const char *start;
	int len;
};

/**
 * Find an optional argument enclosed by squared brackets in the given format string.
 *
 * @param out
 * @param fmt
 * @return fmt beyound the optional arg.
 */
const char *sgit_find_optional_arg(struct optarg *out, const char *fmt)
{
	if (*fmt == '[')
	{
		fmt++;
		out->start = fmt;
		while (*fmt && *fmt != ']')
			fmt++;
		out->len = fmt - out->start;
		if (*fmt)
			fmt++;
	}
	return fmt;
}

/**
 * Prints selected details about the given commit.
 *
 * @param wcommit
 * @param fmt
 */
void print_commit(git_commit *wcommit, const char *fmt)
{
	char c;
	int percent = 0;

	const char *cmsg;
	int nl;
	const git_signature *cauth;
	char oid_str[80];
	char t[40];
	time_t tt;
	struct tm tm;
	const git_oid *woid;
	const git_oid *toid;

	woid = git_commit_id(wcommit);
	toid = git_commit_tree_id(wcommit);
	cmsg  = git_commit_message(wcommit);
	cauth = git_commit_author(wcommit);
	tt = cauth->when.time + cauth->when.offset*60;
	tm = *gmtime(&tt);
	tm.tm_gmtoff = cauth->when.offset*60;
	strftime(t,sizeof(t),"%a %d %b %Y %T %z",&tm);

	while ((c = *fmt++))
	{
		if (!percent)
		{
			if (c == '%')
			{
				percent = 1;
				continue;
			}
		} else
		{
			switch (c)
			{
				case	'a':
						printf("%s <%s>",cauth->name,cauth->email);
						break;
				case	'd':
						printf("%s",t);
						break;
				case	'm':
						{
							nl = 1;
							while ((c = *cmsg++))
							{
								if (nl)
								{
									printf("    ");
									nl = 0;
								}
								putc(c,stdout);
								if (c == '\n') nl = 1;
							}
						}
						break;
				case	'p':
						{
							int i;
							int num_parents = git_commit_parentcount(wcommit);
							struct optarg prefix;
							struct optarg suffix;

							memset(&prefix,0,sizeof(prefix));
							memset(&suffix,0,sizeof(suffix));

							fmt = sgit_find_optional_arg(&prefix, fmt);
							fmt = sgit_find_optional_arg(&suffix, fmt);

							for (i=0;i<num_parents;i++)
							{
								git_commit *pcommit;

								if (git_commit_parent(&pcommit,wcommit,i) == GIT_OK)
								{
									int j;
									for (j=0;j<prefix.len;j++)
										printf("%c",prefix.start[j]);
									printf("%s",git_oid_tostr(oid_str,sizeof(oid_str),git_commit_id(pcommit)));
									for (j=0;j<suffix.len;j++)
										printf("%c",suffix.start[j]);

								}
							}
						}
						break;
				case	'C':
						printf("%s",git_oid_tostr(oid_str,sizeof(oid_str),woid));
						break;
				case	'T':
						printf("%s",git_oid_tostr(oid_str,sizeof(oid_str),toid));
						break;
			}
			percent  = 0;
			continue;
		}
		printf("%c",c);
	}
}

