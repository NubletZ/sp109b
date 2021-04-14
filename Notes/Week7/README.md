# Week 7 Notes
## What is POSIX?
POSIX (Portable Operating System) standard is a  document that released by the IEEE, to clarigy and make uniform the application programming interfaces provided by Unix-y OS. When you write a program based on POSIX standards, you can port them easily in Unix, Linux, and other derivative OS.

## Write C With POSIX
```
// file.c
#include <stdio.h>
#include <unistd.h> //using POSIX
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fd = open("hello.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fd >= 0);
    char buffer[20];
    sprintf(buffer, "hello world!\n");
    int rc = write(fd, buffer, strlen(buffer));
    assert(rc == (strlen(buffer)));
    fsync(fd);
    close(fd);
    return 0;
}
```
The code above is written using POSIX. The function such as open(), close(), fscync() are part from `unistd.h` library which belongs to POSIX. This way you only can compile this program in Unix, Linux, and other derivative OS, you won't be able to compile this in Windows neither in MacOS.

## Write C Program for Linux and Windows
To make a program can run in both Linux and Windows, then you can try to use `ifdef`, `ifndef`, `endif`. This several commands will come in handy to convert some of your program function and the library that you used in a specific condition. I take an example from the code above and modify it a little bit, so that we can run it in Windows.

```
//file.c
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// #define __POSIX__
#ifdef __POSIX__
#define Status 0
#include <unistd.h>
#endif

#ifdef __WINDOWS__
#include <io.h>
#include <math.h>
#define Status 1
#define fsync sqrt //change fsync into sqrt to avoid failed to compile
#define open _open //using Microsoft POSIX-style low-level IO calls
#define close _close //using Microsoft POSIX-style low-level IO calls
                     //ref:https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/open-wopen?view=msvc-160
#endif

FILE *_fd;

int main(int argc, char *argv[]) {
    int fd = open("hello.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IRUSR | _S_IWUSR);
    if (Status == 1) _fd = fopen("hello.txt", "wb");
    assert(fd >= 0);
    char buffer[20];
    sprintf(buffer, "hello world!\n");
    int rc = write(fd, buffer, strlen(buffer));
    assert(rc == (strlen(buffer)));
    if(Status == 0) fsync(fd);
    else fflush(_fd);
    close(fd);
    return 0;
}
```
As you can see from the code above, I separate between `__POSIX__` and `__WINDOWS__`.

```
// #define __POSIX__
#ifdef __POSIX__
#define Status 0
#include <unistd.h>
#endif
```
If the defined value is `__POSIX__` then compiler will set Status into 0 and use `unistd.h` library.
```
#ifdef __WINDOWS__
#include <io.h>
#include <math.h>
#define Status 1
#define fsync sqrt
#define open _open
#define close _close
#endif
```
Otherwise if the defined value is `__WINDOWS__` then this program will :
* use `io.h` and `math.h` library
* set Status into 1
* change open function into _open, and close function into _close. The functions that start with underline are functions from Microsoft POSIX-style low-level I/O calls. This POSIX-style is excutable in Windows.
* change fsync function into sqrt. If we run the program in `__WINDOWS__` we will find an error when compiling the program since it can't recognize the fsync function. Since fd is int, so I change fsync into sqrt which need a float for it's argument. This way it won't meet the same error.