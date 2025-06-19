#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
 
char* fmt_name(char *path){
    static char buf[DIRSIZ+1];
    char *p;
    //find first character after last slash
    for(p=path+strlen(path);p>=path&&*p!='/';p--);
    p++;
    memmove(buf,p,strlen(p)+1);
    return buf;
}
 
void eq_print(char *fileName,char *findName){
    if(strcmp(fmt_name(fileName),findName)==0){
        printf("%s\n",fileName);
    }
}
void find(char *path, char *findName){
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  /*
   int fd;声明一个文件描述符变量 fd，用于在程序中表示打开的文件或目录。
   struct dirent de;定义了一个结构体 dirent 变量 de，通常用于存储读取目录时的目录项信息。
   struct stat st;：定义了一个结构体 stat 变量 st，用于存储文件或目录的状态信息。
   if((fd = open(path, O_RDONLY)) < 0){ ... }：尝试以只读模式打开指定路径的文件或目录。如果 open() 函数返回的文件描述符小于 0（表示打开失败），则输出错误信息，并返回。
   if(fstat(fd, &st) < 0){ ... }：如果文件或目录成功打开，则使用 fstat() 函数获取文件描述符 fd 对应文件或目录的状态信息，并将结果存储在 st 结构体中。如果获取状态信息失败（fstat() 返回值小于 0），则输出错误信息，并关闭文件描述符后返回。  
  */
  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    eq_print(path,findName);
    break;
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    // 读一个目录项
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum ==0||de.inum==1||strcmp(de.name,".")==0 || strcmp(de.name,"..")==0)
        continue;
      memmove(p, de.name, strlen(de.name));
      p[strlen(de.name)] = 0;
      find(buf,findName);
    }
    break;
/*
 循环：遍历目录项 
    read(fd, &de, sizeof(de)):使用read函数从文件描述符 fd中读取一个目录项的内容，并将读取的内容存储在 de 变量中。
    只要 read 函数成功读取一个目录项的大小（sizeof(de)），就会继续执行循环。
 循环内部：
    1. 首先，通过检查 de.inum(目录项的inode号码)是否等于0或1，以及检查目录项的名称是否是.或..，来过滤掉特殊的目录项。
      .（点）表示当前目录,..（点点）表示父目录,它们在此处被跳过,不做进一步处理。
      如果目录项既不是特殊目录项.（点）或 ..（点点），也不是 inode 号码为 0 或 1，则会执行以下操作：
    2. memmove(p, de.name, strlen(de.name));：将目录项的文件名 de.name 复制到一个指定的缓冲区 buf 中，从指针 p 指向的位置开始存储。
      这里使用 memmove 函数是为了确保将文件名正确复制到指定的位置。
      p[strlen(de.name)] = 0;：在复制文件名后，将字符串末尾添加一个空字符 \0，以确保字符串以空字符结尾，形成一个 C 风格的字符串。
      最后，调用 find(buf, findName); 以递归方式在当前路径下的子目录中继续查找名为 findName 的文件或目录。
      在当前目录下的每个子目录中进行深度优先的查找操作。
*/
  }
  close(fd); 
}
/*
void find(char *path, char *findName)：
  递归函数，接收两个参数，path 表示要搜索的路径，findName 表示要查找的文件或目录名。
代码开始通过 open() 函数尝试打开指定的路径，如果失败则会输出错误信息并返回。
使用 fstat() 函数获取文件的状态信息，如果失败也会输出相应的错误信息并关闭文件描述符后返回。
接下来 定义了一个缓冲区buf和指针p，用于构建要查找的文件或目录的完整路径。
使用 switch 语句检查文件类型：
T_FILE：表示当前路径是一个文件，则调用 eq_print() 函数
T_DIR：表示当前路径是一个目录，则进行目录的遍历和递归查找。
  首先检查路径长度是否过长，如果是，则输出相应错误信息。然后将当前路径复制到 buf 中，并在其末尾添加 /。
  接着使用 read() 函数读取目录项，并在循环中遍历目录下的所有文件和子目录。
  在目录遍历的循环中，会检查每个目录项的 inum 值是否为0或1(通常表示未使用的或损坏的inode),以及是否是.或..目录(表示当前目录和父目录).
  如果是则跳过不处理。
将当前目录项的名字添加到 buf 中，并递归调用 find() 函数，以此在当前目录中查找与 findName 匹配的文件或目录。
最后关闭打开的文件描述符 fd。
*/
 
int main(int argc, char *argv[])
{
   if(argc>3){
    printf("find: find <path> <fileName>\n");
    exit(0);
   }
   find(argv[1],argv[2]);
   exit(0);
}