This is a collection of functions that you would likely write yourself when you use POSIX API.  
If you see something that is missing here, you should commit it.


As a quality of life feature this repo also includes:
- stb_ds array and hashmap
- sds dynamic string

### Design

- This prefers C standard over POSIX, this mainly effects thread usage
- There is no additional prefix for these functions

### Modules:
- file.h file and directory utilities
- logc.h syslog utilities and terminal color codes
- timespec.h timespec functions missing from C standard
- delayedcall.h one shot delayed call function in another thread
- semaphore.h c11 thread semaphore
- tpool.h thread pool implementation
- memory.h allocator to help track memory usage


### Usage
See test/ directory for reference.


## build and install
By default PREFIX=/home/$USER/.local

```
make
make install PREFIX=/some/path
```

and remove it with
```
make uninstall 
```
