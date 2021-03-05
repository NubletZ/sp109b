# Week 1 Notes
## GCC
GCC is a collection of programming compilers including various languages such as ada, C, C++, fortran, ObjC, Fortran, and Java.
This GCC is a key component of "GNU Toolchain" that includes GNU Make, an automation tool for compiling and building applications. In this System Program course, GNU Make will often be use to help in compiling program.
## GNU Make
By default, Make will reads a description of a project from a makefile that is in the same current directory. A makefile is a file that specifies a set of compilation rules in term of targets (such as executables) and their dependencies (such as object files and source files).

## Using Make To Compile Program
main.c | sum.c | sum.h
-------|-------|-------
#include <stdio.h><br>#include "sum.h"<br>int main() { <br>int t = sum(10); <br>  printf("sum(10)=%d\n", t); <br>} | #include "sum.h"<br><br>int sum(int n) {<br>int s = 0;<br>for (int i=0; i<=n; i++) {<br>s=s+i;<br>}<br> return s;<br>} | int sum(int n);

Normally, to compile this collection of code we can type the following command :
```
$ gcc -o sum main.c sum.c -I.
```
This will compiles the two .c files and names the executeable program as sum. The -I here have a function to make gcc focus in the curren directory while (.) is typed to include the sum.h file.

The next example will compile program by using makefile to manage the above collection of code
```
//Makefile

CC := gcc
CFLAGS = -std=c99 -O0
TARGET = run

all: $(TARGET)

$(TARGET): sum.c main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o *.exe $(TARGET)
```

and to compile it, simply we can type like this :
```
Nubletz /d/sp/02-gcc/05-make/01-makeExe (master) $ make
gcc -std=c99 -O0 sum.c main.c -o run
```
and to run the executable program we can type like this (e.x the file name is run.exe) :
```
Nubletz /d/sp/02-gcc/05-make/01-makeExe (master) $ ./run
sum(10)=55
```


Reference :

> [GCC and Make Compiling, Linking and Building C/C++ Applications](https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html)

> [A Simple Makefile](https://www.linuxtopia.org/online_books/an_introduction_to_gcc/gccintro_16.html)