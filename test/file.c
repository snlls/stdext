#include <ccpsx/file.h>

#define elog(fmt, ...)                                                                                                 \
	do {                                                                                                           \
		printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);                                          \
	} while(0)

#define FILE_TEST_TARGET "file.test.target"
#define FILE_TEST_DATA "test"

int file_test()
{
	int errors = 0;
	int err;
	if(is_file(FILE_TEST_TARGET)) {
		if((err = unlink(FILE_TEST_TARGET))) {
			elog("failed to unlink test file");
			errors++;
			goto fail;
		}
	}
	if((err = file_write(FILE_TEST_TARGET, FILE_TEST_DATA, false))) {
		elog("%d %s", err, strerror(err));
		errors++;
	}
	if((err = file_write(FILE_TEST_TARGET, FILE_TEST_DATA, true))) {
		elog("%d %s", err, strerror(err));
		errors++;
	}
	int len = strlen(FILE_TEST_DATA);
	size_t l;
	char *fd = file_read(FILE_TEST_TARGET, &l);
	if(l != (size_t)(len * 2)) {
		elog("invalid read count %ld, expected %d", l, len * 2);
		errors++;
	}
	if(!fd) {
		elog("could not read file %s", FILE_TEST_TARGET);
		errors++;
	}
	else if(strcmp(fd, FILE_TEST_DATA FILE_TEST_DATA)) {
		elog("file read does not match got '%s' expected '%s'", fd, FILE_TEST_DATA FILE_TEST_DATA);
		errors++;
	}
	else {
		free(fd);
	}
	unlink(FILE_TEST_TARGET);

fail:
	return errors;
}
