//fileWinLinux.c
//reference : https://gitlab.com/ccc109/sp

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// #define __POSIX__
#ifdef __POSIX__
#define datafd fd
#define Status 0
#include <unistd.h>
#endif

#ifdef __WINDOWS__
#include <io.h>
#include <math.h>
#define Status 1
#define datafd _fd //switch variable that passed since fsync(int) while fflush(FILE)
#define fsync fflush //change fsync into fflush which have a close function for windows
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
    fsync(datafd);
    close(fd);
    return 0;
}