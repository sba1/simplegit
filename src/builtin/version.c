#include "version.h"

#include <stdio.h>

int cmd_version(git_repository *repo, int argc, char **argv)
{
	printf("sgit version 0.%s", SIMPLEGIT_REVISION_STRING);

	return EXIT_SUCCESS;
}
