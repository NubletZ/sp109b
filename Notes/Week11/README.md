# Week 11 Notes
## execvp() and fork() Function in C/C++
### execvp() Function
`execvp()` is a function that enable us to execute a command in our C program. This function will only work in UNIX based OS only and we need to add `<unistd.h>` as the header file if we want to use this function in our C program.
```
int execvp(const char* command, char* argv[]);
```
for an example of C program with `execvp()` function see the code below
```
#include <stdio.h>
#include <unistd.h>

int main() {
  char *arg[] = {"ls", "-l", NULL };
  printf("execvp():before\n");
  execvp(arg[0], arg);
  printf("execvp():after that won't be printed out\n");
}
```
Sample output :
```
Before execvp() function
total 100
drwxrwxr-x 2 NubletZ NubletZ  4096 Apr 14 06:23 backup
-rwxrwxr-x 1 NubletZ NubletZ 16792 May 12 05:51 execvp1
-rw-rw-r-- 1 NubletZ NubletZ   176 Apr 14 06:23 execvp1.c
-rwxrwxr-x 1 NubletZ NubletZ 16840 May 15 14:27 execvpwithFork
-rw-rw-r-- 1 NubletZ NubletZ  1742 May 15 14:26 execvpwithFork.c
-rwxrwxr-x 1 NubletZ NubletZ 16704 May 15 14:50 myexecvp
-rw-rw-r-- 1 NubletZ NubletZ    95 May 15 14:50 myexecvp.c
-rwxrwxr-x 1 NubletZ NubletZ 16800 May 15 16:58 myexecvp2
-rw-rw-r-- 1 NubletZ NubletZ   237 May 15 16:58 myexecvp2.c
```
As you can see that the program won't print the last `printf()` function since the program `ls -l` has took control the process.

### fork() Function
The `fork()` function will creates a new process so you will have two identical processes. See the sample code below :
```
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>

int main() { 
    int p = fork(); //The type is int because fork will return 0
    if (p==0) { //Upon successful completion, fork will return 0 value to the child value
      printf("%-5d: I am child!\n", getpid());
    } else {
      printf("%-5d: I am parent!\n", getpid());
    }
}
```
Output :
```
1423559: I am parent!
nubletz@localhost:~/sp/08-posix/03-fork/02-child$ 1423560: I am child!
```
As you can see from the output, there are two process that running, they are the child process with PID `1423560` that printing "1423560: I am child!" since it already receive 0 value from parent process while the parent process PID is `1423559` and printing "1423559: I am parent!".

### Write C Program with execvp() and fork()
In our last `execvp()` code we successfuly execute the `ls -l` command in C program. But the next function can't be executed because the process has been taken by `execvp()`. By adding `fork()` function, we could execute the command and keep executing the next function.
```
#include <stdio.h>
#include <unistd.h>

int main() {
  int p = fork();
  char *arg[] = {"ls", "-l", NULL };
  if(p == 0){
    printf("entering child process to execute ls -l command\n");
    printf("Before execvp() function\n");
    execvp(arg[0], arg);
    printf("After execvp() this will not be printed\n");
  }
  else {
    printf("This is parent process, will be printed\n");
  }
}
```
Output :
```
This is parent process, will be printed
nubletz@localhost:~/sp/08-posix/03-fork/02-child$ entering child process to execute ls -l command
Before execvp() function
total 52
-rw-rw-r-- 1 NubletZ NubletZ   253 Apr 14 06:23 README.md
-rwxrwxr-x 1 NubletZ NubletZ 16784 May 15 19:07 fork2
-rw-rw-r-- 1 NubletZ NubletZ   351 May 15 19:06 fork2.c
-rwxrwxr-x 1 NubletZ NubletZ 16840 May 15 20:19 forkExecvp
-rw-rw-r-- 1 NubletZ NubletZ   449 May 15 20:19 forkExecvp.c
```
By combining `execvp()` and `fork()` we have separate the program to execute the command by spawning the new process (child process). This way the main function can still running.