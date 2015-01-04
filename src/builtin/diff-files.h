#ifndef GIT_DIFF_FILES_H
#define GIT_DIFF_FILES_H

#include <git2.h>

int cmd_diff_files(git_repository *repo, int argc, char **argv);

#endif
