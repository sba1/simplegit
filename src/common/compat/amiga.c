/* AmigaOS 4 compatiblity functions.
 * (c) 2012 Chris Young <chris@unsatisfactorysoftware.co.uk>
  */

#ifdef __amigaos4__
#include <proto/exec.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "amiga.h"

int pipe(int fd[2])
{
	char fname[30];

	sprintf(fname, "PIPE:%lx%lx%lx", getpid(), time(NULL),  rand());

	fd[0] = fopen(fname, "r");
	fd[1] = fopen(fname, "w");

	return 0;
}

int execvp(const char *file, char *const argv[]) {
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;

	printf("execvp %s\n", file);

	//SystemTags();

	return 0;
}

pid_t waitpid(pid_t pid, int *stat_loc, int options) {
	return 0;
}

#endif
