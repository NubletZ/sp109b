# Week 12 Notes
## I/O system Calls in C Program
### Standard File Descriptors
File descriptor is integer that uniquely identifies an open file of the process. It will return a positive integer for normal return value while a value of -1 is returned when error.

On a Unix-like OS there are mainly three file descriptors :

`0`-`stdin`, stand for standard input

`1`-`stdout`, stand for standard output

`2`-`stderr`, stand for standard error

### I/O System Calls
There are several types of I/O system calls in C including:

#### <b>1). open</b>
This system call is used to access a file in file system for reading, writing, or both.

Syntax :

```
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
int open (const char* Path, int flags [, int mode ]); 
```
Parameters :

* `Path` : the path of a file in directory
* `flags` :
    
    * `O_RDONLY` read only
    * `O_WRONLY` write only
    * `O_RDWR` read and write. It's the same as O_RDONLY | O_WRONLY
    * `O_CREAT` create file if it doesn't exist
    * `O_APPEND` All write operations will be added at the end of file's data

#### <b>2). read</b>
This `read()` function used to read for file data and put it into memory area indicated by buf

Syntax :

```
size_t read (int fd, void* buf, size_t cnt);  
```

Parameters :

* `fd` : file descripter
* `buf` : indicate the memory location
* `cnt` : length of buffer in bytes

#### <b>3). write</b>
the write system call is used to write data from buf into a file associated with fd.

Syntax :

```
#include <fcntl.h>
size_t write (int fd, void* buf, size_t cnt);
```

### Sample Code
After understanding the above system calls now we can see how the code below works, if we have 2 files `a.txt` and empty file `b.txt`

File a.txt
```
hello!
hi
```
File b.txt before executing the program
```
b first line
```

<br>

io2_bak.c code
```
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SMAX 128

int main() {
  int a = open("a.txt", O_RDWR);
  int b = open("b.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
  char line[SMAX];
  int n = read(a, line, SMAX);
  write(b, line, n);
  printf("a=%d, b=%d\n", a, b);
  printf("line=%s\n", line);
}
```
> code reference : https://gitlab.com/ccc109/sp

<br>

Output :
```
nubletz@localhost:~/sp/08-posix/04-fs/00-basic$ ./io2_bak
a=3, b=4
line=hello!
hi
```
<br>

File b.txt after executing the program
```
nubletz@localhost:~/sp/08-posix/04-fs/00-basic$ cat b.txt
b first line
hello!
hi
```