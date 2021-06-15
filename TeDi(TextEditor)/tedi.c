#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)
#define ESC '\x1b'
#define ABUF_INIT {NULL, 0}
#define TEDI_VERSION "0.1"
#define TEDI_TAB_STOP 8

enum moveKey {
/* -- COMMENT --
1. Add large int value to avoid conflict with ordinary keypresses.
2. By setting first constant in enum to 1000, the rest of constants value will be the incementing values.
*/
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY, //escape seq : <esc>[3~
  HOME_KEY, //escape seq: <esc>[1~, <esc>[7~, <esc>{H, or <esc>OH
  END_KEY, //escape seq: <esc[4~, <esc>[8~, <esc>[F, or <esc>OF
  PAGE_UP, //escape seq : <esc>[5~
  PAGE_DOWN //escape seq : <esc>[6~
};

/*** data ***/

typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;
} erow;

struct editorConfig {
  int cx, cy;
  int rx; //cursor position if there is any tabs
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  int numrows;
  erow *row; //define new name for struct erow
  int dirty; //detect whether the text loaded in editor is differs from origin file
  int escstat; //flag if user press esc
  int savewarnstat; //esc but file has been modified
  int esc27; //flag to devide between esc key press and esc return
  char message[80]; //define message at the bottom
  time_t message_time;
  char *filename;
  struct termios orig_termios;
};

struct editorConfig E;

/** prototypes ***/

void editorRefreshScreen();
char *editorPrompt(char *prompt);

/*** terminal ***/

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode); //will be executed automatically when the program exits

  struct termios raw = E.orig_termios;
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

int editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }

  //sprintf(E.message, "c = %c", c);

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) {
      //sprintf(E.message, "enter first c=%d", c);
      E.esc27 = 1;
      return '\x1b';
    }
    if (read(STDIN_FILENO, &seq[1], 1) != 1) {
      //sprintf(E.message, "enter second");
      return '\x1b';
    }
    //sprintf(E.message, "test = %c %c", seq[0], seq[1]);

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) {
          //sprintf(E.message, "enter third");
          return '\x1b';
        }
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      }
      else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    }
    else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return '\x1b';
  }
  else {
    return c;
  }
}

int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1; //argument of 6 to ask for the cursor position

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

  return 0;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
/* --COMMENT --
1. C command : Cursor Forward
2. B command : Cursor Down
unlike H command C and B won't let cursor to past the edge of screen
*/
    return getCursorPosition(rows, cols);
  }
  else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** message below ***/

void setMessage(int type, const char *string, ...) {
  va_list ap;
  va_start(ap, string);
  if (type == 1) {
    snprintf(E.message, sizeof(E.message), "-- EDIT TEXT | %.20s | %d current line %s --", E.filename ? E.filename : "untitled", E.cy + 1, E.dirty ? " (modified)" : "");
  }
  else if (type == 27) {
    sprintf(E.message, "> Do you realy want to exit TeDi? (y/n)");
  }
  else if (type == 28) {
    sprintf(E.message, "> WARNING! File has unsaved changes. Save before exit? (y/n/c)");
  }
  else vsnprintf(E.message, sizeof(E.message), string, ap);
  va_end(ap);
  E.message_time = time(NULL);
}

void messageSaveTime() {
  if (E.escstat != 1) {
    int msglen = strlen(E.message);
    if (msglen && time(NULL) - E.message_time < 5) {
      setMessage(1, NULL);
    }
  }
}

/*** row operations ***/

int editorRowCxToRx(erow *row, int cx) {
//calculate the value of E.rx
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (row->chars[j] == '\t') {
      rx += (TEDI_TAB_STOP - 1) - (rx % TEDI_TAB_STOP);
    }
    rx++;
  }
  return rx;
}

void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') tabs++;
  }

  free(row->render);
  row->render = malloc(row->size + tabs*(TEDI_TAB_STOP - 1) + 1);
/* -- COMMENT --
The maximum number of characters needed for each tab is 8
*/
  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      row->render[idx++] = ' ';
      while (idx % TEDI_TAB_STOP != 0) row->render[idx++] = ' ';
    }
    else {
      row->render[idx++] = row->chars[j]; //copy chars into render
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
}

void editorInsertRow(int at, char *s, size_t len) {
  if (at < 0 || at > E.numrows) return;

  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

  //save each line data
  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize = 0;
  E.row[at].render = NULL;
  editorUpdateRow(&E.row[at]);

  E.numrows++;
  E.dirty++;
}

void editorFreeRow(erow *row) {
  free(row->render);
  free(row->chars);
}

void editorDelRow(int at) {
  if (at < 0 || at >= E.numrows) return;
  editorFreeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
  E.numrows--;
  E.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
  if (at < 0 || at > row->size) at = row->size;
  row->chars = realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at); //used to overwrite the deleted character with the character that come after it
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}

/*** editor operations ***/

void editorInsertChar(int c) {
  if (E.cy == E.numrows) {
    editorInsertRow(E.numrows, "", 0);
  }
  editorRowInsertChar(&E.row[E.cy], E.cx, c);
  E.cx++;
}

void editorInsertNewline() {
  if (E.cx == 0) {
    editorInsertRow(E.cy, "", 0);
  }
  else {
    erow *row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
    row->size = E.cx;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
  }
  E.cy++;
  E.cx = 0;
}

void editorDelChar() {
  if (E.cy == E.numrows) return;
  if (E.cx == 0 && E.cy == 0) return;

  erow *row = &E.row[E.cy];
  if (E.cx > 0) {
    editorRowDelChar(row, E.cx - 1);
    E.cx--;
  }
  else {
    E.cx = E.row[E.cy - 1].size;
    editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
    editorDelRow(E.cy);
    E.cy--;
  }
}

/*** file i/o ***/

char *editorRowsToString(int *buflen) {
//this function will convert array of erow structs into a single string, and write the change into file
  int totlen = 0;
  int j;
  for (j = 0; j < E.numrows; j++) {
    totlen += E.row[j].size + 1;
  }
  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (j = 0; j < E.numrows; j++) {
    memcpy(p, E.row[j].chars, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }

  return buf;
}

void editorOpen(char *filename) {
//used for opening and reading a file from disk
  free(E.filename);
  E.filename = strdup(filename);
  setMessage(1, NULL);

  FILE *fp = fopen(filename, "r");
  if (!fp) die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) linelen--;

    editorInsertRow(E.numrows, line, linelen);
  }
  free(line);
  fclose(fp);
  E.dirty = 0;
}

void editorSave() {
  int tempcy = E.cy;
  int temprx = E.rx;
  if(E.filename == NULL) {
    E.esc27 = 0;
    E.cy = E.screencols+2;
    E.rx = 9;
    E.filename = editorPrompt("Save as: %s (ESC to cancel)");
    if (E.filename == NULL) {
      setMessage(0, "Save aborted");
      E.cy = tempcy;
      E.rx = temprx;
      return;
    }
  }

  int len;
  char *buf = editorRowsToString(&len);

  int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != 1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        E.dirty = 0;
        char msg[80];
        snprintf(msg, sizeof(msg), "-- %d bytes written to disk --", len);
        setMessage(0, msg);
        E.cy = tempcy;
        E.rx = temprx;
        return;
      }
    }
    close(fd);
  }

  
  free(buf);
  setMessage(0, "Can't save the file! I/O error: %s", strerror(errno));
}

/*** append buffer ***/

struct abuf {
  char *b;
  int len;
};

void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}

/*** output ***/

void editorScroll() {
  if (!E.savewarnstat) {
  E.rx = 0;
  if (E.cy < E.numrows) {
    E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
  }
  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows - 1) {
    E.rowoff = E.cy - E.screenrows + 2;
  }
  if (E.rx < E.coloff) {
     E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
  }
}

void editorDrawRows(struct abuf *ab) {
//draw tildes (~) at the begining of each line
  int y;
  for (y = 0; y < E.screenrows - 1; y++) {
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) { //show welcome message (only when TeDi is empty)
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome), "TeDi(Text Editor) -- version %s", TEDI_VERSION);
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) { //if(int) same with if(int =! 0)
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      }
      else {
        abAppend(ab, "~", 1);
      }
    }
    else {
      int len = E.row[filerow].rsize - E.coloff;
      if (len < 0) len = 0;
      if ( len > E.screencols) len = E.screencols;
      abAppend(ab, &E.row[filerow].render[E.coloff], len);
    }

    abAppend(ab, "\x1b[K", 3);
/* -- COMMENT --
1. 3 in write() means we are writing 4 bytes into terminal, \x1b, [, and K.
2. \x1b : esc
3. \x1b[ : escape sequances to instruct terminal to do various text formatting tasks including clearing screen
4. K : erase part of the current line
*/

    if (y < E.screenrows - 1) {
      //only write "\r\n" until before end of line
      abAppend(ab, "\r\n", 2);
    }
  }
  if(y == E.screenrows - 1)
  {
    int messagelen = strlen(E.message);
    int blank = (E.screencols - messagelen - 1);
    abAppend(ab, E.message, messagelen);
    while (blank--) abAppend(ab, " ", 1);
  }
}

void editorRefreshScreen() {
  editorScroll();

  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);

  abAppend(&ab, "\x1b[H", 3);
/* -- COMMENT --
H command is used to position the cursor, in the line above we set cursor position to top-left corner
*/

  editorDrawRows(&ab); //pass ab value so it can be use in editorDrawRows()
  messageSaveTime();

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy -E.rowoff) + 1, (E.rx - E.coloff) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/*** input ***/

char *editorPrompt(char *prompt) {
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';

  while (1) {
    setMessage(0, prompt, buf);
    editorRefreshScreen();

    int c = editorReadKey();
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (buflen != 0) buf[--buflen] = '\0';
      if (E.rx > 9) E.rx--;
    }
    else if (c == '\x1b') {
      setMessage(1, NULL);
      free(buf);
      E.esc27 = 1;
      return NULL;
    }
    else if (c == '\r') {
      if (buflen != 0) {
        setMessage(1, NULL);
        E.esc27 = 1;
        return buf;
      }
    }
    else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
      E.rx++;
    }
    setMessage(1, NULL);
  }
}

void editorMoveCursor(int key) {
  erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
  int temp = E.numrows - 1;

  switch (key) {
    case ARROW_LEFT:
      if (E.cx != 0) E.cx--;
      else if (E.cy > 0) {//go to end of previous line when E.cx == 0 and key press <
        setMessage(1, NULL);
        E.cy--;
        E.cx = E.row[E.cy].size;
      }
      break;
      
    case ARROW_RIGHT:
      if (row && E.cx < row->size) {
        E.cx++;
      }
      else if (row && E.cx == row->size && E.cy < temp) {
        //sprintf(E.message, "%d : %d", E.cy, temp);
        setMessage(1, NULL);
        E.cy++;
        E.cx = 0;
      }
      break;
    case ARROW_UP:
      if (E.cy != 0)  {
        E.cy--;
        setMessage(1, NULL);
      }
      break;
    case ARROW_DOWN:
      if (E.cy < E.numrows - 1) {
        E.cy++;
        setMessage(1, NULL);
      }
      break;
  }

  row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
  int rowlen = row ? row->size : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen; //return E.cx value when it pass rowlen value
  }
}

void editorProcessKeypress() {
  int c = editorReadKey();
  //int temp = E.numrows - 1;

  if (E.esc27 == 1) {
    switch (c) {
      case CTRL_KEY(ESC):
        if (E.dirty == 0) {
          setMessage(27, NULL);
          E.escstat = 1;
        }
        else {
          setMessage(28, NULL);
          E.savewarnstat = 1;
        }
        E.esc27 = 0;
        break;
    }
  }
  if (E.escstat) {
    switch (c) {
      case 'y':
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
      case 'n':
        setMessage(1, NULL);
        E.escstat = 0;
        break;
    }
  }
  if (E.savewarnstat) {
    switch (c) {
      case 'y':
        editorSave();
        editorRefreshScreen();
        sleep(2); //sleep for 2s before quitwrite(STDOUT_FILENO, "\x1b[2J", 4);
      case 'n':
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        E.savewarnstat = 0;
        exit(0);
        break;
     case 'c':
        setMessage(1, NULL);
        E.savewarnstat = 0;
        break;
    }
  }
  else if (!E.escstat && !E.savewarnstat) {
    switch (c) {
      case '\r':
        editorInsertNewline();
        break;

      case CTRL_KEY('s'):
        editorSave();
        break;

      case ARROW_UP:
      case ARROW_DOWN:
      case ARROW_LEFT:
      case ARROW_RIGHT:
        editorMoveCursor(c);
        break;

      case HOME_KEY:
        E.cx = 0;
        break;
      case END_KEY:
        if (E.cy < E.numrows) {
          E.cx = E.row[E.cy].size;
        }
        break;

      case BACKSPACE:
      case CTRL_KEY('h'):
      case DEL_KEY:
        if (c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
        editorDelChar();
        break;

      case PAGE_UP:
      case PAGE_DOWN: //next and prev page
        {
          if ( c == PAGE_UP) {
            E.cy = E.rowoff;
          }
          else if (c == PAGE_DOWN) {
            E.cy = E.rowoff + E.screenrows - 2;
            if (E.cy > E.numrows) E.cy = E.numrows;
          }

          int times = E.screenrows - 2;
          while (times--)
            editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        break;

      case CTRL_KEY('l'):
      case '\x1b':
        break;

      default: //allow keyprass that isn't mapped to be inserted into text
        editorInsertChar(c);
        //sprintf(E.message, "%d : %d", E.cy, temp);
        break;
    }
  }
}

/*** init ***/

void initEditor() {
  E.filename = NULL;
  E.message_time = 0;
  E.cx = 0; //horizontal coordinate of cursor
  E.cy = 0; //vertical coordinate of cursor
  E.rx = 0; //coordinate of cursor when there is any tabs
  E.rowoff = 0;
  E.coloff = 0; 
  E.numrows = 0;
  E.row = NULL;
  E.dirty = 0;
  E.escstat = 0;
  E.esc27 = 0;
  E.savewarnstat = 0;

//initialize screenrows and screencols in E struct
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }
  else setMessage(1, NULL);

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
