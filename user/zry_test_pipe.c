#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p[2];
    char *argv[2];
    argv[0] = "wc";
    argv[1] = 0;

    if (pipe(p) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        // 子进程：关闭标准输入
        close(0);
        // 复制管道读端到标准输入
        if (dup(p[0]) != 0) {
            exit(1);
        }
        // 为了让wc获得标准输入, 再次关闭管道读端
        close(p[0]);
        close(p[1]);

        // 执行 wc 命令
        exec("wc", argv);
        printf("exec failed\n");
        exit(1);
    } else {
        // 父进程：关闭管道读端，向管道写端写入数据
        close(p[0]);
        if (write(p[1], "hello world\n", 12) != 12) {
            exit(1);
        }
        close(p[1]);
        wait(0); // 等待子进程结束
    }
    exit(0);
}
