#include <stdio.h>

#include <git2.h>

#include "print.h"

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
							const char *prefix_start = NULL;
							int prefix_len = 0;
							const char *suffix_start = NULL;
							int suffix_len = 0;

							if (*fmt == '[')
							{
								fmt++;
								prefix_start = fmt;
								while (*fmt && *fmt != ']')
									fmt++;
								prefix_len = fmt - prefix_start;
								if (*fmt) fmt++;
							}

							if (*fmt == '[')
							{
								fmt++;
								suffix_start = fmt;
								while (*fmt && *fmt != ']')
									fmt++;
								suffix_len = fmt - suffix_start;
								if (*fmt) fmt++;
							}

							for (i=0;i<num_parents;i++)
							{
								git_commit *pcommit;

								if (git_commit_parent(&pcommit,wcommit,i) == GIT_OK)
								{
									int j;
									for (j=0;j<prefix_len;j++)
										printf("%c",prefix_start[j]);
									printf("%s",git_oid_tostr(oid_str,sizeof(oid_str),git_commit_id(pcommit)));
									for (j=0;j<suffix_len;j++)
										printf("%c",suffix_start[j]);

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

