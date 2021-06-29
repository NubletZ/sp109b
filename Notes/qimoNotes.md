## Library list
-- unistd.h --
1. `read()`
2. `write()`
3. `STDOUT_FILENO`
4. `ftruncate()`
5. `close()`

-- termios.h --
1. `struct termios`
2. `tcgetattr()`
3. `tcsetattr()`
4. `ECHO`
5. `TCSAFLUSH`
6. `ISIG`
7. `IXON`
8. `ICANON`
9. `IEXTEN`
10. `ICRNL`
11. `OPOST`
12. `BRKINT`, `INPCK`, `ISTRIP`, `CS8` (this several flags are considered to be part of enabling "raw mode") for more info read https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html (step 15)

-- stdlib.h --
1. `atexit()`
2. `exit()`
3. `realloc()`
4. `free()`
5. `malloc()`

-- ctype.h --
1. `iscntrl()`

-- stdio.h --
1. `printf()`
2. `perror()`
3. `sscanf()`
4. `snprintf()`
5. `FILE`
6. `fopen()`
7. `getline()`

-- errno.h --
1. `errno`
2. `EAGAIN`

-- sys/ioctl.h --
1. `ioctl()`
2. `TIOCGWINSZ`
3. `struct winsize`

-- string.h --
1. `memcpy()`
2. `strlen()`
3. `strdup()` : makes copy of the given string, allocating the required memory and assuming you will free() that memory.
4. `memmove()` : like memcpy(), but is safe to use when the source and destination arrays overlap.
5. `strerror()` : is like perror() (which we use in die()), but it takes the errno value as an argument and returns the human-readable string for that error code, so that we can make the error a part of the status message we display to the user.
6. `strstr(const char *haystack, const char *needle)` : haystack is the main C string to be scanned. Needle is the small string to be searched with-in haystack string
memset
7. `strchr()` : looks for the first occurence of a character in a string and returns a pointer to the matching character in the string
8. `strrchr()` : returns a pointer to the last occurrence of a character in a string
9. `strcmp()` : returns 0 if two given strings are equal

-- sys/types.h --
1. `ssize_t`

-- time.h --
1. `time_t`

-- fcntl.h --
1. `open()`
2. `O_RDWR`
3. `O_CREAT`

## ASCII CODE
* 27 : esc
* 9 : tab
* 10 : enter (because of ICRNL， enter will be read as 13)
* 127 : backspace

## short key
* `Ctrl-C` : terminate program
* `Ctrl-V` : wait for input
* `Ctrs-S` : stop data from being transmitted to the terminal until you press Ctrl-Q

## important
1. because we turn off OPOST for all output processing, so we need to write "\r\n" whenever we want to start a new line

2. Explain code :
``` 
write(STDOUT_FILENO, "\x1b[2J", 4);
```
Escape sequences always start with an escape character (27) followed by a [ character. Escape sequences instruct the terminal to do various text formatting tasks, such as coloring text, moving the cursor around, and clearing parts of the screen.

3. About clearing screen :
The argument is 2, which says to clear the entire screen. <esc>[1J would clear the screen up to where the cursor is, and <esc>[0J would clear the screen from the cursor up to the end of the screen. Also, 0 is the default argument for J, so just <esc>[J by itself would also clear the screen from the cursor to the end.

4. H command
The H command actually takes two arguments: the row number and the column number at which to position the cursor. So if you have an 80×24 size terminal and you want the cursor in the center of the screen, you could use the command <esc>[12;40H. (Multiple arguments are separated by a ; character.) The default arguments for H both happen to be 1, so we can leave both arguments out and it will position the cursor at the first row and first column, as if we had sent the <esc>[1;1H command. (Rows and columns are numbered starting at 1, not 0.)

5. need to be noted :
We could use atexit() to clear the screen when our program exits, but then the error message printed by die() would get erased right after printing it.

6. The result from requesting cursor position (argument of 6) would look something like : 24;80R

7. need to be noted :
Maybe you noticed the last line of the screen doesn’t seem to have a tilde. That’s because of a small bug in our code. When we print the final tilde, we then print a "\r\n" like on any other line, but this causes the terminal to scroll in order to make room for a new, blank line. Let’s make the last line an exception when we print our "\r\n"’s.

8. Pay attention :
in our code most of write() are changed into abAppend, here is the reason https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#append-buffer

9. Line code : 
```
abAppend(&ab, "\x1b[2J", 4);
```
-- COMMENT --
> * 4 in write() means we are writing 4 bytes into terminal, \x1b, [, 2, and J.
> * \x1b : esc
> * \x1b[ : escape sequances to instruct terminal to do various text formatting tasks including clearing screen
> * J : command to clear the screen
> * 2 : means to clear the entire screen


10. The Home key could be sent as <esc>[1~, <esc>[7~, <esc>[H, or <esc>OH. Similarly, the End key could be sent as <esc>[4~, <esc>[8~, <esc>[F, or <esc>OF. Let’s handle all of these cases.

11. typedef struct function in c :
https://www.educative.io/edpresso/how-to-use-the-typedef-struct-in-c

12. Know the structure from memcpy() :
```
void *memcpy(void *dest, const void * src, size_t n)
```
Parameters :
> * dest - pointer to the destination array
> * src - pointer to the source of data (file to be copied)
> * n - number of bytes to be copied

13. getline() is useful for reading lines from a file when we don’t know how much memory to allocate for each line. It takes care of memory management for you. First, we pass it a null line pointer and a linecap (line capacity) of 0. That makes it allocate new memory for the next line it reads, and set line to point to the memory, and set linecap to let you know how much memory it allocated.

14. How getline() works:
The while loop works because getline() returns -1 when it gets to the end of the file and there are no more lines to read.

15. The dot (.) operator is used to access a member of a struct, while the arrow operator (->) in C is used to access a member of a struct which is referenced by the pointer in question.
(https://stackoverflow.com/questions/13366083/why-does-the-arrow-operator-in-c-exist)

16. Let’s call editorInsertChar() in the default: case of the switch statement in editorProcessKeypress(). This will allow any keypress that isn’t mapped to another editor function to be inserted directly into the text being edited.
