期末作業
=======
> * 主題 : TeDi (Text Editor) 編輯器
> * 作者 : 魏美亞
> * 學號 : 110810538
> * 作品 : 並非100%原創
> * Demo : https://github.com/NubletZ/sp109b/blob/nubletz/Notes/demoVideo.md#demo-teditext-editor
> * Reference : https://viewsourcecode.org/snaptoken/kilo/
> * This program is still on progress, the basic functions have finished but I still need to add several features soon.

I write TeDi text editor in order to fulfill my final assignment with reference to [Build Your Own Text Editor](https://viewsourcecode.org/snaptoken/kilo/). I do a little modification to it and write a Makefile to enable Tedi to be installed in your system in root mode which I will explain [below](https://github.com/NubletZ/sp109b/blob/nubletz/TeDi(TextEditor)/README.md#part-that-i-modified).

In order to use TeDi you need to clone this project, open it in your terminal and execute this command : `$ make`. After that you will find tedi file in your directory. to run it simply type `$ ./tedi [filename]` if you are root you have an option to install Tedi in your system, this way you'll able to run TeDi in any directory by simply type `tedi [filename]`.

There are several shortcut that you need to know :
1. `ESC` : exit program
2. `Ctrl` + `s` : SaveAs/Save file
3. `Ctrl` + `f` : find strings in file

I'm still doing the function which enable user to select some string and use `ctrl + c` to copy the string and paste it using `ctrl + v`. Up until now for this function I have made a progress until user can highlight the string that they want to copy. The user need to press `ctrl + e` to start choosing the string and press `ctrl + e` again to stop.
<br>

![gifDemoHL](https://user-images.githubusercontent.com/62128475/123844111-362fb880-d945-11eb-89e0-b5c492ddef75.gif)

<br>


Up until now, here is some function list in TeDi :
1. Make new empty file by typing `$ ./tedi` or `$ tedi`
2. SaveAs/save the file
3. Scroll between page by using `Page Up` and `Page Down`
4. Show how many bytes that written into disk on save file
5. Message to confirm that you want to leave TeDi
6. Status that show if you have done some modification into your file
7. If you have modified the file but want to leave the program there would be a message to ask you whether you want to save the file or not.
8. Status that show your current cursor line position
9. Different color highlight on certain filetype
10. An option to install TeDi in system (need root mode)

# Part that I modified
## Text Editor C Code
### status bar and status message
In showing the status bar and status message, I only use the last line in screen and reverse the color. In this function I separate the message into several types depends on the conditions :
1. The default message would need the int type value 1. It will show your filename, filetype, at which line your cursor is and the total line, and last whether you have modified the file or not.
2. The second type is when the user want to close Tedi, it will show the message to make sure whether the user really want to close the program or not.
3. The third one is a warning message that would be shown when the user want to exit the program but the file has been modified and hasn't been saved yet. So it will ask the user whether they want to save it first before exit the program or not.
4. The last is a function which enable me to set the message string by define the type value 0.

```
void setMessage(int type, const char *string, ...) {
  va_list ap;
  va_start(ap, string);
  if (type == 1) {
    snprintf(E.message, sizeof(E.message), "-- EDIT TEXT | %.20s (%.8s) | line %d/%d %s --", E.filename ? E.filename : "untitled", E.syntax ? E.syntax->filetype : "no type", E.cy + 1, E.numrows, E.dirty ? " (modified)" : "");
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
```
### Highlighting text and background
Whith the code from [Viewsourcecode](https://viewsourcecode.org/snaptoken/kilo/) it would enable you to highlight the text by changing the text color. But I want to change the background color into yellow when the user search for a specific string, so I add this several line code :

```
if (color < 40 || color > 49 ) clen = snprintf(buf, sizeof(buf), "\x1b[%dm\x1b[49m", color);
```
this way if I change the color background of scpecific string, it won't affect the other string background color. Also don't forget to add the code to reset the text color and background color :

```
abAppend(ab, "\x1b[49m", 5);
abAppend(ab, "\x1b[39m", 5);
```
49 to reset the background color and 39 to reset the text color. Ofcourse you can write it as one line code :

```
abAppend(ab, "\x1b[39m\x1b[49m", 10);
```

<br>

Last if you pay enough attention in my code there are some condition that would return `\x1b` that equal to `ESC`, to avoid my program processing this input and terminate the program I make a flag that will differentiate between `ESC` that only a return value and `ESC` from keypress.

## Makefile
I write my own Makefile so that it has the function that I want. At first the program will check whether you are a user or a root. If you are a root then the `%EUID` value will return 0, and it will ask you whether you want to install Tedi in your system or in you current working directory. While if you are a user then the program will directly install the program in your current working directory since user don't have access to install a program into `/usr/bin`. To remove Tedi from current working directory use `$make clean` while to uninstall it us `#make uninstall`

```
CC=gcc
CFLAGS=-I.

$(VERBOSE).SILENT:

.PHONY: install clean uninstall

all: install

install: tedi.c
	if [[ $EUID -ne 0 ]] ; \
	then \
		echo "You are not root" ; \
		echo "Installing Tedi in your current directory.." ; \
		$(CC) tedi.c -o tedi -Wall -Wextra -pedantic -std=c99 -lpthread ; \
	else \
		echo "You are root" ; \
		while [ -z "$$CONTINUE" ] ; do \
			read -r -p "Do you want to install Tedi in your system? [y/N]: " CONTINUE ; \
		done ; \
		[ $$CONTINUE = "y" ] || [ $$CONTINUE = "Y" ] || (echo "Installing Tedi in your current directory.."; $(CC) tedi.c -o tedi -Wall -Wextra -pedantic -std=c99 -lpthread;) ; \
		[ $$CONTINUE = "n" ] || [ $$CONTINUE = "N" ] || (echo "Installing Tedi in your system.." ; $(CC) tedi.c -o "/bin/tedi" -Wall -Wextra -pedantic -std=c99 -lpthread;) ; \
	fi

clean:
	echo "Cleaning up.." ; \
	rm "tedi" ; \
	if [[ $$? -eq 0 ]] ; \
	then \
		echo "Tedi has been removed" ; \
	else \
		echo "Failed to remove Tedi" ; \
	fi

uninstall:
	echo "Uninstalling Tedi.." ; \
	rm "/bin/tedi" ; \
	if [[ $$? -eq 0 ]] ; \
	then \
		echo "Tedi has been uninstalled" ; \
	else \
		echo "Failed to uninstall Tedi" ; \
	fi
```

Here is the link for [my notes](https://github.com/NubletZ/sp109b/blob/nubletz/Notes/qimoNotes.md) during I write this code. But it's so messy, maybe I will tidy it up later to make it readable(?) haha. Thank you and please let me know if you found some mistake or something else. I'm still a student that need to learn and learn :)
