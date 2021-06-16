# Week 13 Notes
## FIFO Named Pipe
FIFO is a named pipe and it is one of the methods for intern-process communication. In FIFO any number of readers and writers may use the pipe. This FIFO can life in the process as long as the system is up and it can be deleted if no longer used.

This special file can be entered into the filesystem by calling mkfifo(). This function need `sys/types.h` and `sys/stat.h` as its headers. To create the FIFO file in our program we can type the following code

```
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
```

in Linux we also can use a `$ mkfifo` command to create a named pipe.

<img src="MakePipe.PNG" alt="Make named pipe" title="Make named pipe" width="900">

Next we will use this `pipe1` to build an internal communication. Put the process to read mypipe2 in the background and you will recieve the PID of it.

```
$ cat < mypipe2&
[1] 1660436
```

Use `$ echo` and type a string as an input for `mypipe2`. After execute this command, your string would be printed by the process `cat` that you run before to read `mypipe2`. After success printing the content it will be terminated.

```
$ echo "testing" > mypipe2
$ testing

[1]+  Done                    cat < mypipe2
```

## Why Use FIFO?
There's some situation that you might need FIFO to solve the problem. One situation you've got a backup script that runs via [cron](https://github.com/NubletZ/LinuxOS_Notes/tree/nubletz/109%E4%B8%8B/Week-15#crontab), and after it's finished, you need to shutdown the system. If you shutdown  from the backup script, then cron will never sees the backup script finish, so it never sends out the e-mail containing the output from the backup job. You can run another cron job that will shutdown the system at the time after the backup is "supposed" to finish, but this way have a chance that the cron will shutting down too early and will need you to set a larger value for delay time.

By using a FIFO, you can start the backup and the shutdown cron jobs at the same time and have the shutdown just wait untill the backkup writes to the FIFO. When cron job reads something from pipe, it will pauses for a few minutes to send the e-mail, and then it will shut down the system.

In another case if you have a backup that wakes up every hour or so and reads a named pipe to see if it should run. You could write the names of the files that you want backed up to the pipe so the backup doesn't have to check everything.

> This part take reference to [Linux Journal - Using Named Pipes (FIFOs) with Bash](https://www.linuxjournal.com/content/using-named-pipes-fifos-bash)