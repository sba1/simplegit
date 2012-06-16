#ifndef __COMMON_H__
#define __COMMON_H__

#include <git2.h>

int ls_remote(int argc, char **argv);
int parse_pkt_line(int argc, char **argv);
int show_remote(int argc, char **argv);
int fetch(int argc, char **argv);
int index_pack(int argc, char **argv);
int clone(int argc, char **argv);

#endif /* __COMMON_H__ */
