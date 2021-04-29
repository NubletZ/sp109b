# Week 9 Notes
## Process and Thread
Process means that a program is in execution. An operating system can have more than one process that running in it. Each process runs independently and it will have their own address space. In one process, it can have many threads, each thread in process shares the same memory and resources.

## Creating Threads
When a program starts executing it will executes the main() function. A new thread also can be added to the program by using `pthread_create()` function. Look at the sample code below :

```
// CODE REFERENCE : https://gitlab.com/ccc109/sp/

#include <pthread.h>     // 引用 pthread 函式庫
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> 

void *print_george(void *argu) {    // 每隔一秒鐘印出一次 George 的函數
  while (1) {    
    printf("George\n");    
    sleep(1);    
  }    
  return NULL;    
}    

void *print_mary(void *argu) {     // 每隔2秒鐘印出一次 Mary 的函數
  while (1) {    
    printf("Mary\n");    
    sleep(2);    
  }    
  return NULL;    
}    

int main() {     // 主程式開始
  pthread_t thread1, thread2;     // 宣告兩個執行緒
  pthread_create(&thread1, NULL, &print_george, NULL);    // 執行緒 print_george
  pthread_create(&thread2, NULL, &print_mary, NULL);    // 執行緒 print_mary
  while (1) {     // 主程式每隔一秒鐘
    printf("----------------\n");    // 就印出分隔行
    sleep(1);     // 停止一秒鐘
  }    
  return 0;    
}
```

from the code above, 2 threads with `pthread_t` type were declared in `main()` function, they are thread1 and thread2. thread1 refer to `print_george()` and thread2 refer to `print_mary()`. After declared, we can call `pthread_create` function to make the thread living. There are 4 arguments in `pthread_create()`, the pointer, argument to set some thread attributes (NULL for default values), thread name, and an arbitrary argument. After all of the thread has been created, now the program will cosist of three threads, since the main program is also a thread and will execute the `main()` function in parallel to the threads it creates. Note that the `sleep()` function in this program is used to causes the calling thread to `sleep` for a few second and then resume execution.

## Do you know?
### Deadlock
Deadlock is when the program cannot access a resouce it needs to continue the program. An application may would be unresponsive when a deadlock occurs. For example see the code below :

```
// CODE REFERENCE : https://gitlab.com/ccc109/sp/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t x;
pthread_mutex_t y;

void *A(); 
void *B(); 

int main(int argc, char *argv[])
{
    pthread_t threadA, threadB;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_mutex_init(&x, NULL);
    pthread_mutex_init(&y, NULL);

    pthread_create(&threadA, &attr, A, NULL);
    pthread_create(&threadB, &attr, B, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    pthread_mutex_destroy(&x);
    pthread_mutex_destroy(&y);
}

void *A() 
{
    pthread_mutex_lock(&x);
    printf("A lock x\n");

    sleep(1);
    pthread_mutex_lock(&y);
    printf("A lock y\n");

    pthread_mutex_unlock(&y); 
    pthread_mutex_unlock(&x); 

    printf("finished A\n");

    pthread_exit(0);
}

void *B()
{
    
    pthread_mutex_lock(&y);
    printf("B lock y\n");
    sleep(1);
    pthread_mutex_lock(&x);
    printf("B lock x\n");
    pthread_mutex_unlock(&x);
    pthread_mutex_unlock(&y);

    pthread_exit(0);
}

```

process "a" need to lock "y" and process "b" need to lock "x", but from the code above, both "x" and "y" have already been locked. So the program start to be unresponsive because it keep waiting for "x" and "y".

### Race Condition
A race condition is a condition where in program a thread was run or finished at an unexpected time, so it may cause unpredictable behavior, such as incorrect output or a deadlock. For example if the correct process should be
<ol><li>Load variable X</li> <li>Load variable Y</li> <li> Check result of X + Y</li></ol>
but in race condition the program may run in incorrect sequence instead:
<ol><li>Load variable X</li> <li> Check result of X + Y</li> <li>Load variable Y</li></ol>
The result from the incorrect sequence of function may result an inconsistence output because the variable Y has not been loaded yet whe the function start to do the operation X + Y.