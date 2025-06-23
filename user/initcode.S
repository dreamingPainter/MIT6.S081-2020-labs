# Initial process that execs /init.
# This code runs in user space.

#include "syscall.h"

# exec(init, argv)
.globl start
start:
        la a0, init     # load address
        la a1, argv
        li a7, SYS_exec # system call id
        ecall

# for(;;) exit();
exit:
        li a7, SYS_exit
        ecall
        jal exit        # 跳转到exit标签

# char init[] = "/init\0";
init:
  .string "/init\0"     # init程序

# char *argv[] = { init, 0 };
.p2align 2              # 对齐到4字节边界
argv:                   
  .long init            # 定义一个4字节的长整型, 值为init的值
  .long 0               # 定义一个4字节的长整型，值为0，表示数组结束
