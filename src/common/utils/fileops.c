#include "errors.h"
#include "fileops.h"
#include <git2.h>
#include <ctype.h>


/*
 * Based on the Android implementation, BSD licensed.
 * Check http://android.git.kernel.org/
 */
int dirname_r(char *buffer, size_t bufflen, const char *path)
{
	const char *endp;
	int result, len;
	
	/* Empty or NULL string gets treated as "." */
	if (path == NULL || *path == '\0') {
		path = ".";
		len  = 1;
		goto Exit;
	}

	/* Strip trailing slashes */
	endp = path + strlen(path) - 1;
	while (endp > path && *endp == '/')
		endp--;

	/* Find the start of the dir */
	while (endp > path && *endp != '/')
		endp--;

	/* Either the dir is "/" or there are no slashes */
	if (endp == path) {
		path = (*endp == '/') ? "/" : ".";
		len  = 1;
		goto Exit;
	}

	do {
		endp--;
	} while (endp > path && *endp == '/');

	len = endp - path +1;

	Exit:
	result = len;
	if (len+1 > GIT_PATH_MAX) {
		return GIT_ERROR;
	}
	if (buffer == NULL)
		return result;

	if (len > (int)bufflen-1) {
		len    = (int)bufflen-1;
		result = GIT_ERROR;
	}

	if (len >= 0) {
		memmove(buffer, path, len);
		buffer[len] = 0;
	}
	return result;
}

void joinpath_n(char *buffer_out, int count, ...)
{
	va_list ap;
	int i;
	char *buffer_start = buffer_out;
	
	va_start(ap, count);
	for (i = 0; i < count; ++i) {
		const char *path;
		int len;
		
		path = va_arg(ap, const char *);
		
		assert((i == 0) || path != buffer_start);
		
		if (i > 0 && *path == '/' && buffer_out > buffer_start && buffer_out[-1] == '/')
			path++;
		
		if (!*path)
			continue;
		
		len = strlen(path);
		memmove(buffer_out, path, len);
		buffer_out = buffer_out + len;
		
		if (i < count - 1 && buffer_out[-1] != '/')
			*buffer_out++ = '/';
	}
	va_end(ap);
	
	*buffer_out = '\0';
}

void joinpath(char *buffer_out, const char *path_a, const char *path_b)
{
	joinpath_n(buffer_out, 2, path_a, path_b);
}


int git2_mkdir_2file(const char *file_path)
{
	const int mode = 0755; /* or 0777 ? */
	int error = GIT_OK;
	char target_folder_path[GIT_PATH_MAX];
	
	error = dirname_r(target_folder_path, sizeof(target_folder_path), file_path);
	
	if (error < GIT_OK)
		return GIT_ERROR;
	
	/* Does the containing folder exist? */
	if (git2_isdir(target_folder_path)) {
		joinpath(target_folder_path, target_folder_path, ""); /* Ensure there's a trailing slash */
		
		/* Let's create the tree structure */
		error = git2_mkdir_recurs(target_folder_path, mode);
		if (error < GIT_OK)
			return error;	/* The callee already takes care of setting the correct error message. */
	}
	
	return GIT_OK;
}

//int git2_mktemp(char *path_out, const char *filename)
//{
//	int fd;
//
//	strcpy(path_out, filename);
//	strcat(path_out, "_git2_XXXXXX");
//
//#if defined(_MSC_VER)
//	/* FIXME: there may be race conditions when multi-threading
//	 * with the library */
//	if (_mktemp_s(path_out, GIT_PATH_MAX) != 0)
//		return git__throw(GIT_EOSERR, "Failed to make temporary file %s", path_out);
//
//	fd = git2_creat(path_out, 0744);
//#else
//	fd = mkstemp(path_out);
//#endif
//
//	return fd >= 0 ? fd : git__throw(GIT_EOSERR, "Failed to create temporary file %s", path_out);
//}
//
//int git2_open(const char *path, int flags)
//{
//	int fd = open(path, flags | O_BINARY);
//	return fd >= 0 ? fd : git__throw(GIT_EOSERR, "Failed to open %s", path);
//}
//
int git2_creat(const char *path, int mode)
{
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
	return fd;
}


int remove_dir_recursively(struct strbuf *path)
{
	DIR *dir;
	struct dirent *e;
	int ret = 0, original_len = path->len, len;
	
	dir = opendir(path->buf);
	if (!dir)
		return rmdir(path->buf);
	if (path->buf[original_len - 1] != '/')
		strbuf_addch(path, '/');
	
	len = path->len;
	while ((e = readdir(dir)) != NULL) {
		struct stat st;
		if (strcmp(e->d_name, "..") == 0 || strcmp(e->d_name, ".")==0)
			continue;
		
		strbuf_setlen(path, len);
		strbuf_addstr(path, e->d_name);
		if (lstat(path->buf, &st))
			; /* fall thru */
			else if (S_ISDIR(st.st_mode)) {
				if (!remove_dir_recursively(path))
					continue; /* happy */
			} else if (!unlink(path->buf))
				continue; /* happy, too */
				
				/* path too long, stat fails, or non-directory still exists */
				ret = -1;
			break;
	}
	closedir(dir);
	
	strbuf_setlen(path, original_len);
	if (!ret)
		ret = rmdir(path->buf);
	return ret;
}

int git2_isdir(const char *path)
{
	struct stat st;
	int len, stat_error;

	if (!path)
		return GIT_ENOTFOUND;

	len = strlen(path);

	/* win32: stat path for folders cannot end in a slash */
	if (path[len - 1] == '/') {
		char *path_fixed = NULL;
		path_fixed = strdup(path);
		if (path_fixed == NULL)
			return GIT_ERROR;
		path_fixed[len - 1] = 0;
		stat_error = git2_stat(path_fixed, &st);
		free(path_fixed);
	} else {
		stat_error = git2_stat(path, &st);
	}

	if (stat_error < GIT_OK)
		return GIT_ENOTFOUND;

	if (!S_ISDIR(st.st_mode))
		return GIT_ENOTFOUND;

	return GIT_OK;
}

int git2_exists(const char *path)
{
	assert(path);

	struct stat st;
	return git2_lstat(path, &st);
}
//
//git_off_t git2_size(git_file fd)
//{
//	struct stat sb;
//	if (git2_fstat(fd, &sb))
//		return git__throw(GIT_EOSERR, "Failed to get size of file. File missing or corrupted");
//	return sb.st_size;
//}
//
//int git2_read_file(git2_buf *obj, const char *path)
//{
//	git_file fd;
//	size_t len;
//	git_off_t size;
//	unsigned char *buff;
//
//	assert(obj && path && *path);
//
//	if ((fd = git2_open(path, O_RDONLY)) < 0)
//		return git__throw(GIT_ERROR, "Failed to open %s for reading", path);
//
//	if (((size = git2_size(fd)) < 0) || !git__is_sizet(size+1)) {
//		git2_close(fd);
//		return git__throw(GIT_ERROR, "Failed to read file `%s`. An error occured while calculating its size", path);
//	}
//	len = (size_t) size;
//
//	if ((buff = git__malloc(len + 1)) == NULL) {
//		git2_close(fd);
//		return GIT_ENOMEM;
//	}
//
//	if (git2_read(fd, buff, len) < 0) {
//		git2_close(fd);
//		free(buff);
//		return git__throw(GIT_ERROR, "Failed to read file `%s`", path);
//	}
//	buff[len] = '\0';
//
//	git2_close(fd);
//
//	obj->data = buff;
//	obj->len  = len;
//
//	return GIT_OK;
//}
//
//void git2_free_buf(git2_buf *obj)
//{
//	assert(obj);
//	free(obj->data);
//	obj->data = NULL;
//}
//
//int git2_mv(const char *from, const char *to)
//{
//	int error;
//
//#ifdef GIT_WIN32
//	/*
//	 * Win32 POSIX compilance my ass. If the destination
//	 * file exists, the `rename` call fails. This is as
//	 * close as it gets with the Win32 API.
//	 */
//	error = MoveFileEx(from, to, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) ? GIT_OK : GIT_EOSERR;
//#else
//	/* Don't even try this on Win32 */
//	if (!link(from, to)) {
//		git2_unlink(from);
//		return GIT_OK;
//	}
//
//	if (!rename(from, to))
//		return GIT_OK;
//
//	error = GIT_EOSERR;
//#endif
//
//	if (error < GIT_OK)
//		return git__throw(error, "Failed to move file from `%s` to `%s`", from, to);
//
//	return GIT_OK;
//}
//
//int git2_mv_force(const char *from, const char *to)
//{
//	if (git2_mkdir_2file(to) < GIT_OK)
//		return GIT_EOSERR;	/* The callee already takes care of setting the correct error message. */
//
//	return git2_mv(from, to);	/* The callee already takes care of setting the correct error message. */
//}
//
//int git2_map_ro(git_map *out, git_file fd, git_off_t begin, size_t len)
//{
//	if (git__mmap(out, len, GIT_PROT_READ, GIT_MAP_SHARED, fd, begin) < GIT_OK)
//		return GIT_EOSERR;
//	return GIT_OK;
//}
//
//void git2_free_map(git_map *out)
//{
//	git__munmap(out);
//}
//
//int git2_dirent(
//	char *path,
//	size_t path_sz,
//	int (*fn)(void *, char *),
//	void *arg)
//{
//	size_t wd_len = strlen(path);
//	DIR *dir;
//	struct dirent *de;
//
//	if (!wd_len || path_sz < wd_len + 2)
//		return git__throw(GIT_EINVALIDARGS, "Failed to process `%s` tree structure. Path is either empty or buffer size is too short", path);
//
//	while (path[wd_len - 1] == '/')
//		wd_len--;
//	path[wd_len++] = '/';
//	path[wd_len] = '\0';
//
//	dir = opendir(path);
//	if (!dir)
//		return git__throw(GIT_EOSERR, "Failed to process `%s` tree structure. An error occured while opening the directory", path);
//
//	while ((de = readdir(dir)) != NULL) {
//		size_t de_len;
//		int result;
//
//		/* always skip '.' and '..' */
//		if (de->d_name[0] == '.') {
//			if (de->d_name[1] == '\0')
//				continue;
//			if (de->d_name[1] == '.' && de->d_name[2] == '\0')
//				continue;
//		}
//
//		de_len = strlen(de->d_name);
//		if (path_sz < wd_len + de_len + 1) {
//			closedir(dir);
//			return git__throw(GIT_ERROR, "Failed to process `%s` tree structure. Buffer size is too short", path);
//		}
//
//		strcpy(path + wd_len, de->d_name);
//		result = fn(arg, path);
//		if (result < GIT_OK) {
//			closedir(dir);
//			return result;	/* The callee is reponsible for setting the correct error message */
//		}
//		if (result > 0) {
//			closedir(dir);
//			return result;
//		}
//	}
//
//	closedir(dir);
//	return GIT_OK;
//}
//
//int git2_lock_exclusive(int fd)
//{
//	return flock(fd, LOCK_EX);
//}
//
//int git2_lock_shared(int fd)
//{
//	return flock(fd, LOCK_SH);
//}
//
//int git2_unlock(int fd)
//{
//	return flock(fd, LOCK_UN);
//}
//
static void posixify_path(char *path)
{
	#if GIT_PLATFORM_PATH_SEP != '/'
		while (*path) {
			if (*path == GIT_PLATFORM_PATH_SEP)
				*path = '/';

			path++;
		}
	#endif
}

int git2_retrieve_path_root_offset(const char *path)
{
	int offset = 0;

#ifdef GIT_WIN32

	/* Does the root of the path look like a windows drive ? */
	if (git2_has_dos_drive_prefix(path))
		offset += 2;

#endif

	if (*(path + offset) == '/')
		return offset;

	return -1;	/* Not a real error. Rather a signal than the path is not rooted */
}
//
//int git2_retrieve_path_ceiling_offset(const char *path, const char *prefix_list)
//{
//	assert(path);
//
//	char buf[GIT_PATH_MAX + 1];
//	char buf2[GIT_PATH_MAX + 1];
//	const char *ceil, *sep;
//	int len, max_len = -1;
//	int min_len = git2_retrieve_path_root_offset(path) + 1;
//
//	if (prefix_list == NULL || !strcmp(path, "/"))
//		return min_len;
//
//	for (sep = ceil = prefix_list; *sep; ceil = sep + 1) {
//		for (sep = ceil; *sep && *sep != GIT_PATH_LIST_SEPARATOR; sep++);
//		len = sep - ceil;
//
//		if (len == 0 || len > GIT_PATH_MAX || !git2_is_absolute_path(ceil))
//			continue;
//
//		memcpy(buf, ceil, len+1);
//		buf[len + 1] = '\0';
//
//		posixify_path(buf);
//		if (git2_prettify_dir_path(buf2, sizeof(buf2), buf) < GIT_OK)
//			continue;
//
//		len = strlen(buf2);
//		if (len > 0 && buf2[len-1] == '/')
//			buf[--len] = '\0';
//
//		if (!strncmp(path, buf2, len) &&
//			path[len] == '/' &&
//			len > max_len)
//		{
//			max_len = len;
//		}
//	}
//
//	return max_len <= min_len ? min_len : max_len;
//}
//
//
int git2_mkdir_recurs(const char *path, int mode)
{
	int error, root_path_offset;
	char *pp, *sp;
	char *path_copy = strdup(path);

	if (path_copy == NULL)
		return GIT_ERROR;

	error = GIT_OK;
	pp = path_copy;

	root_path_offset = git2_retrieve_path_root_offset(pp);
	if (root_path_offset > 0)
		pp += root_path_offset; /* On Windows, will skip the drive name (eg. C: or D:) */

	while (error == GIT_OK && (sp = strchr(pp, '/')) != 0) {
		if (sp != pp && git2_isdir(path_copy) < GIT_OK) {
			*sp = 0;
			error = git2_mkdir(path_copy, mode);

			/* it's a not a directory */
			if (errno == EEXIST) {
				if (unlink(path_copy) != 0)
					return GIT_ERROR;
				error = git2_mkdir(path_copy, mode);
			}

			*sp = '/';
		}

		pp = sp + 1;
	}

	if (*(pp - 1) != '/' && error == GIT_OK)
		error = git2_mkdir(path, mode);

	free(path_copy);

	if (error < GIT_OK)
		return error;

	return GIT_OK;
}

static int git2_retrieve_previous_path_component_start(const char *path)
{
	int offset, len, root_offset, start = 0;

	root_offset = git2_retrieve_path_root_offset(path);
	if (root_offset > -1)
		start += root_offset;

	len = strlen(path);
	offset = len - 1;

	/* Skip leading slash */
	if (path[start] == '/')
		start++;

	/* Skip trailing slash */
	if (path[offset] == '/')
		offset--;

	if (offset < root_offset)
		return GIT_ERROR;

	while (offset > start && path[offset-1] != '/') {
		offset--;
	}

	return offset;
}

int git2_prettify_dir_path(char *buffer_out, size_t size, const char *path)
{
	int len = 0, segment_len, only_dots, root_path_offset, error = GIT_OK;
	char *current;
	const char *buffer_out_start, *buffer_end;

	current = (char *)path;
	buffer_end = path + strlen(path);
	buffer_out_start = buffer_out;

	root_path_offset = git2_retrieve_path_root_offset(path);
	if (root_path_offset < 0) {
		error = git2_getcwd(buffer_out, size);
		if (error < GIT_OK)
			return error;	/* The callee already takes care of setting the correct error message. */

		len = strlen(buffer_out);
		buffer_out += len;
	}

	while (current < buffer_end) {
		/* Prevent multiple slashes from being added to the output */
		if (*current == '/' && len > 0 && buffer_out_start[len - 1] == '/') {
			current++;
			continue;
		}
		
		only_dots = 1;
		segment_len = 0;

		/* Copy path segment to the output */
		while (current < buffer_end && *current != '/')
		{
			only_dots &= (*current == '.');
			*buffer_out++ = *current++;
			segment_len++;
			len++;
		}

		/* Skip current directory */
		if (only_dots && segment_len == 1)
		{
			current++;
			buffer_out -= segment_len;
			len -= segment_len;
			continue;
		}

		/* Handle the double-dot upward directory navigation */
		if (only_dots && segment_len == 2)
		{
			current++;
			buffer_out -= segment_len;

			*buffer_out ='\0';
			len = git2_retrieve_previous_path_component_start(buffer_out_start);

			/* Are we escaping out of the root dir? */
			if (len < 0)
				return GIT_ERROR;

			buffer_out = (char *)buffer_out_start + len;
			continue;
		}

		/* Guard against potential multiple dot path traversal (cf http://cwe.mitre.org/data/definitions/33.html) */
		if (only_dots && segment_len > 0)
			return GIT_ERROR;

		*buffer_out++ = '/';
		len++;
	}

	*buffer_out = '\0';

	return GIT_OK;
}

int git2_prettify_file_path(char *buffer_out, size_t size, const char *path)
{
	int error, path_len, i;
	const char* pattern = "/..";

	path_len = strlen(path);

	/* Let's make sure the filename isn't empty nor a dot */
	if (path_len == 0 || (path_len == 1 && *path == '.'))
		return GIT_ERROR;

	/* Let's make sure the filename doesn't end with "/", "/." or "/.." */
	for (i = 1; path_len > i && i < 4; i++) {
		if (!strncmp(path + path_len - i, pattern, i))
			return GIT_ERROR;
	}

	error =  git2_prettify_dir_path(buffer_out, size, path);
	if (error < GIT_OK)
		return error;	/* The callee already takes care of setting the correct error message. */

	path_len = strlen(buffer_out);
	int root_offset = git2_retrieve_path_root_offset(buffer_out);
	if (root_offset >= 0 && path_len <= root_offset + 1)
		return GIT_ERROR;

	/* Remove the trailing slash */
	buffer_out[path_len - 1] = '\0';

	return GIT_OK;
}
//
//int git2_cmp_path(const char *name1, int len1, int isdir1,
//		const char *name2, int len2, int isdir2)
//{
//	int len = len1 < len2 ? len1 : len2;
//	int cmp;
//
//	cmp = memcmp(name1, name2, len);
//	if (cmp)
//		return cmp;
//	if (len1 < len2)
//		return ((!isdir1 && !isdir2) ? -1 :
//                        (isdir1 ? '/' - name2[len1] : name2[len1] - '/'));
//	if (len1 > len2)
//		return ((!isdir1 && !isdir2) ? 1 :
//                        (isdir2 ? name1[len2] - '/' : '/' - name1[len2]));
//	return 0;
//}
//

int git2_getcwd(char *buffer_out, size_t size)
{
	char *cwd_buffer;

	assert(buffer_out && size > 0);

#ifdef GIT_WIN32
	cwd_buffer = _getcwd(buffer_out, size);
#else
	cwd_buffer = getcwd(buffer_out, size);
#endif

	if (cwd_buffer == NULL)
		return GIT_ERROR;

	posixify_path(buffer_out);

	git2__joinpath(buffer_out, buffer_out, "");	//Ensure the path ends with a trailing slash

	return GIT_OK;
}

//int git2_realpath(const char *path, char *buffer_out)
//{
//	assert(buffer_out);
//
//	if (!realpath(path, buffer_out))
//		git__throw(GIT_EOSERR, "Failed to retrieve real path: %s causing errors", buffer_out);
//
//	posixify_path(buffer_out);
//
//	return GIT_OK;
//}
//
//dev_t git2_retrieve_device(const char *path, dev_t *device)
//{
//	assert(device);
//
//	struct stat path_info;
//	if (git2_stat(path, &path_info))
//		return git__throw(GIT_EOSERR, "Failed to get file informations: %s", path);
//
//	*device = path_info.st_dev;
//
//	return GIT_OK;
//}
