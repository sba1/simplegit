#ifndef __COMMON_H__
#define __COMMON_H__

#include <git2.h>

typedef int (*git_cb)(git_repository *, int , char **);

int ls_remote(git_repository *repo, int argc, char **argv);
int parse_pkt_line(git_repository *repo, int argc, char **argv);
int show_remote(git_repository *repo, int argc, char **argv);
int fetch(git_repository *repo, int argc, char **argv);
int index_pack(git_repository *repo, int argc, char **argv);
int do_clone(git_repository *repo, int argc, char **argv);


/**
 * Instantiates the signature for the author.
 *
 * @param author_signature
 * @return
 */
int sgit_get_author_signature(git_signature **author_signature);

/**
 * Instantiates the signature for the committer.
 *
 * @param committer_signature
 * @return
 */
int sgit_get_committer_signature(git_signature** committer_signature);

int cred_acquire_cb(git_cred **out,
		const char * url,
		const char * username_from_url,
		unsigned int allowed_types,
		void *payload);


/**
 * Function that can be passed to the remote certificate callbacks.
 *
 * @param cert
 * @param valid
 * @param host
 * @param payload
 * @return
 */
int certificate_check(git_cert *cert, int valid, const char *host, void *payload);

#ifndef PRIuZ
/* Define the printf format specifer to use for size_t output */
#if defined(_MSC_VER) || defined(__MINGW32__)
#	define PRIuZ "Iu"
#else
#	define PRIuZ "zu"
#endif
#endif

#endif /* __COMMON_H__ */
