#include "common.h"

#include <stdio.h>

#include "environment.h"
#include "utils/date.h"

/* Shamelessly borrowed from http://stackoverflow.com/questions/3417837/
 * with permission of the original author, Martin Pool.
 * http://sourcefrog.net/weblog/software/languages/C/unused.html
 */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

int cred_acquire_cb(git_cred **out,
		const char * UNUSED(url),
		const char * UNUSED(username_from_url),
		unsigned int UNUSED(allowed_types),
		void * UNUSED(payload))
{
	char username[128] = {0};
	char password[128] = {0};

	printf("Username: ");
	scanf("%s", username);

	/* Yup. Right there on your terminal. Careful where you copy/paste output. */
	printf("Password: ");
	scanf("%s", password);

	return git_cred_userpass_plaintext_new(out, username, password);
}

int certificate_check(git_cert *cert, int valid, const char *host, void *payload)
{
	char *val;
	int ssl_no_verify = 0;

	if (valid)
		return 1;

	if (!(val = getenv("GIT_SSL_NO_VERIFY")))
		return 0;

	if (git_config_parse_bool(&ssl_no_verify, val) < 0)
		return 0;

	if (ssl_no_verify)
		fprintf(stderr, "Ignoring that certificate could not be verified as GIT_SSL_NO_VERIFY variable is set.\n");
	return ssl_no_verify;
}

int sgit_get_author_signature(git_repository *repo, git_signature **author_signature)
{
	int err;
	int author_offset = 0;
	char *author_name = NULL;
	char *author_email = NULL;
	char *author_date = NULL;
	unsigned long author_timestamp = 0;

	err = git_signature_default(author_signature, repo);
	if (err == 0)
		return 0;
	if (err != GIT_ENOTFOUND)
		return err;

	author_name = getenv(GIT_AUTHOR_NAME_ENVIRONMENT);
	author_email = getenv(GIT_AUTHOR_EMAIL_ENVIRONMENT);
	author_date = getenv(GIT_AUTHOR_DATE_ENVIRONMENT);
	if (!author_name)
		author_name = "Dummy Author";

	if (!author_email)
		author_email = "dummya@dummydummydummy.zz";

	if (author_date)
	{
		if ((parse_date_basic(author_date, &author_timestamp, &author_offset)))
		{
			fprintf(stderr, "Bad author date format\n!");
			return GIT_ERROR;
		}
	}

	if (!author_timestamp)
	{
		return git_signature_now(author_signature, author_name, author_email);
	} else
	{
		return git_signature_new(author_signature, author_name, author_email,
			author_timestamp, author_offset);
	}
}

int sgit_get_committer_signature(git_repository *repo, git_signature** committer_signature)
{
	int err;
	char *committer_name = NULL;
	char *committer_email = NULL;
	char *committer_date = NULL;
	unsigned long committer_timestamp = 0;
	int committer_offset = 0;

	err = git_signature_default(committer_signature, repo);
	if (err == 0)
		return 0;
	if (err != GIT_ENOTFOUND)
		return err;

	committer_name = getenv(GIT_COMMITTER_NAME_ENVIRONMENT);
	committer_email = getenv(GIT_COMMITTER_EMAIL_ENVIRONMENT);
	committer_date = getenv(GIT_COMMITTER_DATE_ENVIRONMENT);
	if (!committer_name)
		committer_name = "Dummy Committerr";

	if (!committer_email)
		committer_email = "dummyc@dummydummydummy.zz";

	if (committer_date)
	{
		if ((parse_date_basic(committer_date, &committer_timestamp, &committer_offset)))
		{
			fprintf(stderr, "Bad committer date format\n!");
			return GIT_ERROR;
		}
	}
	if (!committer_timestamp)
	{
		return git_signature_now(committer_signature, committer_name, committer_email);
	} else
	{
		return git_signature_new(committer_signature, committer_name,
				committer_email, committer_timestamp, committer_offset);
	}
}

