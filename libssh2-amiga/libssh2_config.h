#ifndef _LIBSSH2_CONFIG_H
#define _LIBSSH2_CONFIG_H

#include <proto/bsdsocket.h>

#undef m_len

#include <sys/ioctl.h>

#define HAVE_FIONBIO
#define HAVE_O_NONBLOCK

#include <stdio.h>

#endif

