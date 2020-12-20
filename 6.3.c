/*
6.3 Вывод содержимого каталога.
путь к каталогу в виде ./aa/bbb/
аргумент строки - имя каталога
*/
#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

char* checkType ( unsigned char type )
{
if ( type == DT_LNK ) 
return "symbolic link";

else if ( type == DT_DIR )
return "directory";

else if ( type == DT_CHR )
return "character device";

else if ( type == DT_BLK )
return "block device";

else if ( type == DT_FIFO )
return "fifo";

else if ( type == DT_SOCK )
return "socket";

else if ( type == DT_REG )
return "regular file";

else
return  "unknown file's type";
}


struct linux_dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int main(int argc, char* argv[])
{   
    const unsigned int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    char *dir_path;
    char entry_path[PATH_MAX - 1];
    char dir_type;
    struct linux_dirent *linux_dir;

    if ( argc >= 2 )
    {
        dir_path = argv[1];
    } 
    else
    {
        dir_path = ".";
    }

    size_t len_path = strlen ( dir_path );

    strncpy ( entry_path, dir_path, len_path );
    
    if ( entry_path[len_path - 1] != '/' )
    {
        entry_path[len_path] = '/';
        entry_path[len_path + 1] = '\0';
        len_path++;
    }

    int fd_dir = open ( entry_path, O_RDONLY | O_DIRECTORY );
    if ( fd_dir == -1 )
    {
        perror ("Failed to get fd dir ");
        return -1;
    }

    int pos = 0;

    for ( ; ; )
    {
        int read_bytes = syscall( SYS_getdents, fd_dir, buf, sizeof(char)*BUF_SIZE );

        if ( read_bytes == -1)
        {
            fprintf ( stderr, "syscall getdents error\n" );
            return -1;
        }

        if ( read_bytes == 0 )
        {
            break;
        }

        printf("--------------- all bytes: = %d ---------------\n", read_bytes);
        printf("i-node    file type    d_reclen    d_off    d_name\n");
        
        for ( pos = 0; pos < read_bytes; ) 
        {
            linux_dir = (struct linux_dirent *) (buf + pos);

            printf("%9ld  ", linux_dir->d_ino);

            dir_type = *(buf + pos + linux_dir->d_reclen - 1); // там хранится байт, отвечающий за тип в структуре
            
            printf("%-12s ", checkType ( dir_type ) );
            printf("%5d %10lld  %s\n", linux_dir->d_reclen, (long long)linux_dir->d_off, linux_dir->d_name);
            pos += linux_dir->d_reclen; // перемещаем указатель на следующую структуру
        }
        
    }

    return 0;
}
