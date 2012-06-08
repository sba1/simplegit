/* AmigaOS 4 compatiblity functions.
 * (c) 2012 Chris Young <chris@unsatisfactorysoftware.co.uk>
  */

#ifndef COMMON_COMPAT_AMIGA_H
#define COMMON_COMPAT_AMIGA_H

#ifdef __amigaos4__
int pipe(int fd[2]);
int execvp(const char *file, char *const argv[]);
#endif


#endif /* COMMON_COMPAT_AMIGA_H */
