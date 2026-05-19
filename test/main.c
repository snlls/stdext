#include <stdio.h>

int timespec_test();
int semaphore_test();
int delayedcall_test();
int memory_test();
int tpool_test();
int file_test();
int log_test();
int objpool_test();

int main()
{
	int errors = 0;
	int err;

	err = log_test();
	printf("log errors %d\n", err);
	errors += err;
	err = timespec_test();
	printf("timespec errors %d\n", err);
	errors += err;
	err = memory_test();
	printf("memory errors %d\n", err);
	errors += err;
	err = delayedcall_test();
	printf("delayedcall errors %d\n", err);
	errors += err;
	err = tpool_test();
	printf("tpool errors %d\n", err);
	errors += err;

	err = file_test();
	printf("file errors %d\n", err);
	errors += err;
	
	err = objpool_test();
	printf("objpool errors %d\n", err);
	errors += err;
	
	for(int n = 0; n < 5; n++) {
		printf("semaphore test run %d\n", n);
		err = semaphore_test();
		if(err) {
			printf("semaphore errors %d run=%d\n", err, n + 1);
			break;
		}
	}
	errors += err;
	if(!errors) {
		printf("\nall tests passed\n");
	}
	return errors;
}
