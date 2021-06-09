#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/*** data ***/

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

/*** terminal ***/

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode); //will be executed automatically when the program exits

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); 
/* -- COMMENT --
Input flag :
1. ~(ICRNL) will make Ctrl-M read as 13 instead of 10
2. ~(IXON) disables Ctrl-S
*/

  raw.c_oflag &= ~(OPOST);
/* -- COMMENT --
Output flag :
1. ~(OPOST) : turning off all output processing 
*/

  raw.c_cflag |= (CS8);

  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
/* -- COMMENT --
Local flag :
1. ~(ECHO) will tell program to not print the keyboard input
2. ~(ICANON) is a flag that will tell program to read input byte-by-byte, instead of line-by-line
3. ~(IEXTEN) will disable Ctrl-V
4. ~(ISIG) will turn off Ctrl-C and Ctrl-Z signals (it also disables Ctrl-Y in macOS)
*/

  raw.c_cc[VMIN] = 0; //return each byte, else zero for timeout
  raw.c_cc[VTIME] = 1; //100 ms timeout

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); // TCSAFLUSH will discards any unread input before applying the changes to the terminal
}

/*** init ***/

int main() {
  enableRawMode();

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    }
    else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;
  }
  return 0;
}
