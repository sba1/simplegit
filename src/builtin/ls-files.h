#ifndef GIT_LS_FILES_H
#define GIT_LS_FILES_H

#include <git2.h>

int cmd_ls_files(git_repository *repo, int argc, char **argv);

#endif
