期末作業
=======
> * 主題 : TeDi (Text Editor) 編輯器
> * 作者 : 魏美亞
> * 學號 : 110810538
> * 作品 : 並非100%原創
> * Reference : https://viewsourcecode.org/snaptoken/kilo/
> * This program is still on progress, the basic functions have finished but I still need to add several features soon.

I write TeDi text editor in order to fulfill my final assignment with reference to [Build Your Own Text Editor](https://viewsourcecode.org/snaptoken/kilo/). I do a little modification to it which I will explain after I finished writing the program.

In order to use TeDi you need to open this directory in your terminal and execute this command : `$ make`. After that you will find tedi file in your directory. to run it simply type `$ ./tedi [filename]`.

There are several shortcut that you need to know :
1. `ESC` : exit program
2. `Ctrl` + `s` : SaveAs/Save file

Up until now, here is some function list in TeDi :
1. Make new empty file by typing `$ ./tedi`
2. SaveAs/save the file
3. Scroll between page by using `Page Up` and `Page Down`
4. Show how many bytes that written into disk on save file
5. Message to confirm that you want to leave TeDi
6. Status that show if you have done some modification into your file
7. If you have modified the file but want to leave the program there would be a message to ask you whether you want to save the file or not.
8. Status that show your current cursor row position