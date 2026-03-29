#include "stdext/file.h"
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

char *file_read(const char *path, size_t *size)
{
	char *ret = NULL;
	if(!path)
		return NULL;
	FILE *f = fopen(path, "r");
	if(!f)
		return NULL;
	if(fseek(f, 0, SEEK_END))
		goto end;
	long pos = ftell(f);
	if(pos < 0)
		goto end;

	rewind(f);

	// add null terminator
	size_t buflen = pos + 1;
	ret = calloc(1, buflen);
	if(!ret) {
		goto end;
	}

	char *p = ret;
	long rcount = 0;
	while(1) {
		size_t tmp = fread(ret, 1, pos, f);
		if(!tmp) {
			break;
		}
		rcount += tmp;
		p += tmp;
		pos -= tmp;
		if(!pos) {
			break;
		}
	}
	if(size)
		*size = rcount;
end:
	fclose(f);
	return ret;
}

int file_write(const char *path, const char *str, bool append)
{
	FILE *f = fopen(path, append ? "a" : "w");

	int err = 0;
	if(!f) {
		return errno;
	}
	size_t len = strlen(str);
	char *p = (char *)str;
	while(1) {
		size_t wcount = fwrite(p, 1, len, f);
		if(!wcount) {
			err = errno;
			break;
		}
		len -= wcount;
		if(!len) {
			break;
		}
		p += wcount;
	}
	fclose(f);
	return err;
}

bool is_file(const char *path)
{
	if(!path)
		return 0;

	struct stat st = {0};
	if(stat(path, &st)) {
		return 0;
	}
	return S_ISREG(st.st_mode);
}
bool is_dir(const char *path)
{
	if(!path)
		return 0;

	struct stat st = {0};
	if(stat(path, &st)) {
		return 0;
	}
	return S_ISDIR(st.st_mode);
}

bool is_symbolic_link(const char *path)
{
	if(!path)
		return 0;

	struct stat st = {0};
	if(stat(path, &st)) {
		return 0;
	}
	return S_ISLNK(st.st_mode);
}
