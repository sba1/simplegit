#ifndef GIT_SUPPORT_H
#define GIT_SUPPORT_H

char *please_git_help_me(const char **argv);
//execute a git command and returns the results as a string.
//You have to free the returned string when you don't need it
//anymore

#ifndef __amigaos4__
void please_git_do_it_for_me() __attribute__((noreturn));
#else
void please_git_do_it_for_me();
#endif
//substitute the current executable to git
//and execute the command call (registered by main
//at the program call with git_support_register_arguments)

void git_support_register_arguments(int argc, const char **argv);
//register the command call

void git_support_free_arguments();
//free registered arguments

#endif
