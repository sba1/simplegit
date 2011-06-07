/*
 * fileops.h - OS agnostic disk io operations
 * Copied from libgit2
 *
 */
#ifndef INCLUDE_fileops_h__
#define INCLUDE_fileops_h__

#include "utils.h"
#include <string.h>
//#include "common.h"
//#include "map.h"
//#include "dir.h"
//#include <fcntl.h>
//#include <time.h>
//
#include "strbuf.h"

#define GIT_PATH_MAX 4096

#define GIT_PATH_LIST_SEPARATOR ':'

#ifdef GIT__WIN32
#define GIT_PLATFORM_PATH_SEP '/'
#else
#define GIT_PLATFORM_PATH_SEP '\\'
#endif


/**
 * Join two paths together. Takes care of properly fixing the
 * middle slashes and everything
 *
 * The paths are joined together into buffer_out; this is expected
 * to be an user allocated buffer of `GIT_PATH_MAX` size
 */
extern void joinpath_n(char *buffer_out, int npath, ...);

extern void joinpath(char *buffer_out, const char *path_a, const char *path_b);
extern int remove_dir_recursively(struct strbuf *path);


#ifdef GIT_WIN32
GIT_INLINE(int) link(const char *GIT_UNUSED(old), const char *GIT_UNUSED(new))
{
	GIT_UNUSED_ARG(old)
	GIT_UNUSED_ARG(new)
	errno = ENOSYS;
	return -1;
}

//GIT_INLINE(int) git__mkdir(const char *path, int GIT_UNUSED(mode))
//{
//	GIT_UNUSED_ARG(mode)
//	return mkdir(path);
//}

//extern int git__unlink(const char *path);
//extern int git__mkstemp(char *template);
//extern int git__fsync(int fd);

//# ifndef GIT__WIN32_NO_HIDE_FILEOPS
//#  define unlink(p) git__unlink(p)
//#  define mkstemp(t) git__mkstemp(t)
//#  define mkdir(p,m) git__mkdir(p, m)
//#  define fsync(fd) git__fsync(fd)
//# endif
#endif  /* GIT_WIN32 */


#if !defined(O_BINARY)
#define O_BINARY 0
#endif

//#define GITFO_BUF_INIT {NULL, 0}
//
//typedef int git_file;
//
//typedef struct {  /* file io buffer  */
//	void *data;  /* data bytes   */
//	size_t len;  /* data length  */
//} git2_buf;
//
extern int git2_exists(const char *path);
//extern int git2_open(const char *path, int flags);
extern int git2_creat(const char *path, int mode);
extern int git2_creat_force(const char *path, int mode);
//extern int git2_mktemp(char *path_out, const char *filename);
extern int git2_isdir(const char *path);
extern int git2_mkdir_recurs(const char *path, int mode);
extern int git2_mkdir_2file(const char *path);
//#define git2_close(fd) close(fd)
//extern int git2_lock_exclusive(int fd);
//extern int git2_lock_shared(int fd);
//extern int git2_unlock(int fd);
//
//extern int git2_read(git_file fd, void *buf, size_t cnt);
//extern int git2_write(git_file fd, void *buf, size_t cnt);
//#define git2_lseek(f,n,w) lseek(f, n, w)
//extern git_off_t git2_size(git_file fd);
//
//extern int git2_read_file(git2_buf *obj, const char *path);
//extern void git2_free_buf(git2_buf *obj);
//
///* Move (rename) a file; this operation is atomic */
//extern int git2_mv(const char *from, const char *to);
//
///* Move a file (forced); this will create the destination
// * path if it doesn't exist */
//extern int git2_mv_force(const char *from, const char *to);
//
#define git2_stat(p,b) stat(p, b)
#define git2_fstat(f,b) fstat(f, b)
//extern dev_t git2_retrieve_device(const char *path, dev_t *device);
#define git2_lstat(p,b) lstat(p,b)
//
#define git2_unlink(p) unlink(p)
#define git2_rmdir(p) rmdir(p)
#define git2_chdir(p) chdir(p)
#define git2_mkdir(p,m) mkdir(p, m)

//#define git2_mkstemp(t) mkstemp(t)
//#define git2_fsync(fd) fsync(fd)
//#define git2_chmod(p,m) chmod(p, m)
//
///**
// * Read-only map all or part of a file into memory.
// * When possible this function should favor a virtual memory
// * style mapping over some form of malloc()+read(), as the
// * data access will be random and is not likely to touch the
// * majority of the region requested.
// *
// * @param out buffer to populate with the mapping information.
// * @param fd open descriptor to configure the mapping from.
// * @param begin first byte to map, this should be page aligned.
// * @param end number of bytes to map.
// * @return
// * - GIT_SUCCESS on success;
// * - GIT_EOSERR on an unspecified OS related error.
// */
//extern int git2_map_ro(
//	git_map *out,
//	git_file fd,
//	git_off_t begin,
//	size_t len);
//
///**
// * Release the memory associated with a previous memory mapping.
// * @param map the mapping description previously configured.
// */
//extern void git2_free_map(git_map *map);
//
///**
// * Walk each directory entry, except '.' and '..', calling fn(state).
// *
// * @param pathbuf buffer the function reads the initial directory
// * 		path from, and updates with each successive entry's name.
// * @param pathmax maximum allocation of pathbuf.
// * @param fn function to invoke with each entry.  The first arg is
// *		the input state and the second arg is pathbuf.  The function
// *		may modify the pathbuf, but only by appending new text.
// * @param state to pass to fn as the first arg.
// */
//extern int git2_dirent(
//	char *pathbuf,
//	size_t pathmax,
//	int (*fn)(void *, char *),
//	void *state);
//
//extern int git2_cmp_path(const char *name1, int len1, int isdir1,
//		const char *name2, int len2, int isdir2);
//
extern int git2_getcwd(char *buffer_out, size_t size);
//
/**
 * Clean up a provided absolute or relative directory path.
 * 
 * This prettification relies on basic operations such as coalescing 
 * multiple forward slashes into a single slash, removing '.' and 
 * './' current directory segments, and removing parent directory 
 * whenever '..' is encountered.
 *
 * If not empty, the returned path ends with a forward slash.
 *
 * For instance, this will turn "d1/s1///s2/..//../s3" into "d1/s3/".
 *
 * This only performs a string based analysis of the path.
 * No checks are done to make sure the path actually makes sense from 
 * the file system perspective.
 *
 * @param buffer_out buffer to populate with the normalized path.
 * @param size buffer size.
 * @param path directory path to clean up.
 * @return
 * - GIT_SUCCESS on success;
 * - GIT_ERROR when the input path is invalid or escapes the current directory.
 */
int git2_prettify_dir_path(char *buffer_out, size_t size, const char *path);

/**
 * Clean up a provided absolute or relative file path.
 * 
 * This prettification relies on basic operations such as coalescing 
 * multiple forward slashes into a single slash, removing '.' and 
 * './' current directory segments, and removing parent directory 
 * whenever '..' is encountered.
 *
 * For instance, this will turn "d1/s1///s2/..//../s3" into "d1/s3".
 *
 * This only performs a string based analysis of the path.
 * No checks are done to make sure the path actually makes sense from 
 * the file system perspective.
 *
 * @param buffer_out buffer to populate with the normalized path.
 * @param size buffer size.
 * @param path file path to clean up.
 * @return
 * - GIT_SUCCESS on success;
 * - GIT_ERROR when the input path is invalid or escapes the current directory.
 */
int git2_prettify_file_path(char *buffer_out, size_t size, const char *path);

int git2_retrieve_path_root_offset(const char *path);
//
///*
// * path = Canonical absolute path
// * prefix_list = Colon-separated list of absolute paths
// *
// * Determines, for each path in prefix_list, whether the "prefix" really
// * is an ancestor directory of path.  Returns the length of the longest
// * ancestor directory, excluding any trailing slashes, or
// * git2_retrieve_path_ceiling_offset(path) + 1 if no prefix
// * is an ancestor.  (Note that this means 1 is returned if prefix_list is
// * "/".) "/foo" is not considered an ancestor of "/foobar".  Directories
// * are not considered to be their own ancestors.  path must be in a
// * canonical form: empty components, or "." or ".." components are not
// * allowed.  prefix_list may be null, which is like "".
// */
//int git2_retrieve_path_ceiling_offset(const char *path, const char *prefix_list);
//
///*
// * Compute an absolute symlinks free and posixified path from a given path (must exists).
// * Posixified means that '/' are used as directory separators.
// * The entered path must exists in the filesystem, and may be relative or absolute.
// *
// * @param path path to be resolved (must exists in the filesystem).
// * @param buffer_out buffer to populate with the normalized path
// * (must be at least GIT_PATH_MAX long)
// * @return
// * - GIT_SUCCESS on success;
// * - GIT_ERROR on error (and then buffer_out contains the path causing the error)
// */
//int git2_realpath(const char *path, char *buffer_out);
//
#ifdef GIT__WIN32
GIT_INLINE(int) git2_has_dos_drive_prefix(const char path)
{
	return isalpha(path[0]) && (path[1] == ':');
}
#endif
//
//GIT_INLINE(int) git2_is_absolute_path(const char *path)
//{
//	#ifdef GIT__WIN32
//		return git2_has_dos_drive_prefix(path);
//	#else
//		return path[0] == '/';
//	#endif
//}
#endif /* INCLUDE_fileops_h__ */
