#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char** argv) {
    // 父进程向子进程
    int f2c[2];
    // 子进程向父进程
    int c2f[2];

    if (pipe(f2c) < 0 || pipe(c2f) < 0) {
        printf("create pipe fail\n");
    }
    int pid = fork();
    // 父进程
    if (pid != 0) {
        // 父进程向子进程发送一个字符
        write(f2c[1], ".", 1);
        close(f2c[1]);
        // 从子进程读一个字符
        char buf;
        read(c2f[0], &buf, 1);
        printf("%d: receive pong\n", getpid());
        wait(0);
    }else {
        char buf;
        read(f2c[0], &buf, 1);
        // 子进程向父进程写一个
        write(c2f[1], &buf, 1);
        printf("%d:receive ping\n", getpid());;
        close(c2f[1]);
    }
    exit(0);
}