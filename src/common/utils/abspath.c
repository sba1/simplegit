#include "abspath.h"
#include "utils.h"
#include "errors.h"

/*
 * Do not use this for inspecting *tracked* content.  When path is a
 * symlink to a directory, we do not want to say it is a directory when
 * dealing with tracked content in the working tree.
 */
int is_directory(const char *path)
{
	struct stat st;
	return (!stat(path, &st) && S_ISDIR(st.st_mode));
}

/* We allow "recursive" symbolic links. Only within reason, though. */
#define MAXDEPTH 5

/*
 * Use this to get the real path, i.e. resolve links. If you want an
 * absolute path but don't mind links, use absolute_path.
 *
 * If path is our buffer, then return path, as it's already what the
 * user wants.
 */
const char *real_path(const char *path)
{
	static char bufs[2][PATH_MAX + 1], *buf = bufs[0], *next_buf = bufs[1];
	char cwd[1024] = "";
	int buf_index = 1;

	int depth = MAXDEPTH;
	char *last_elem = NULL;
	struct stat st;

	/* We've already done it */
	if (path == buf || path == next_buf)
		return path;

	if (strlcpy(buf, path, PATH_MAX) >= PATH_MAX)
		die ("Too long path: %.*s", 60, path);

	while (depth--) {
		if (!is_directory(buf)) {
			char *last_slash = strrchr(buf, '/');
			if (last_slash) {
				*last_slash = '\0';
				last_elem = xstrdup(last_slash + 1);
			} else {
				last_elem = xstrdup(buf);
				*buf = '\0';
			}
		}

		if (*buf) {
			if (!*cwd && !getcwd(cwd, sizeof(cwd)))
				die_errno ("Could not get current working directory");

			if (chdir(buf))
				die_errno ("Could not switch to '%s'", buf);
		}
		if (!getcwd(buf, PATH_MAX))
			die_errno ("Could not get current working directory");

		if (last_elem) {
			size_t len = strlen(buf);
			if (len + strlen(last_elem) + 2 > PATH_MAX)
				die ("Too long path name: '%s/%s'",
						buf, last_elem);
			if (len && buf[len-1] != '/')
				buf[len++] = '/';
			strcpy(buf + len, last_elem);
			free(last_elem);
			last_elem = NULL;
		}

		if (!lstat(buf, &st) && S_ISLNK(st.st_mode)) {
			ssize_t len = readlink(buf, next_buf, PATH_MAX);
			if (len < 0)
				die_errno ("Invalid symlink '%s'", buf);
			if (PATH_MAX <= len)
				die("symbolic link too long: %s", buf);
			next_buf[len] = '\0';
			buf = next_buf;
			buf_index = 1 - buf_index;
			next_buf = bufs[buf_index];
		} else
			break;
	}

	if (*cwd && chdir(cwd))
		die_errno ("Could not change back to '%s'", cwd);

	return buf;
}
