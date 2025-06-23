// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;
  // 创建新的控制台设备文件，本来一般
  if(open("console", O_RDWR) < 0){  // 尝试打开新的控制台设备文件
    mknod("console", CONSOLE, 0);   // 失败则创建控制条设备文件
    open("console", O_RDWR);        // 返回一个文件描述符，返回控制台文件描述符0
  }
  dup(0);  // stdout, 让标准输出重定向到控制台,占用了stdout
  dup(0);  // stderr，让标准错误重定向到控制台,占用了stderr
  // 让标准输入、输出、错误都输出到控制台
  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    // 子进程用来执行shell程序
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
