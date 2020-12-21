/*
6.3 Вывод содержимого каталога.
путь к каталогу в виде ./aa/bbb/
аргумент строки - имя каталога
*/
#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

const unsigned int BUF_SIZE = (2 << 9);

#define ASN AT_SYMLINK_NOFOLLOW

const char* dType ( unsigned char type )
{
    if ( type == DT_LNK ) 
        return "s.lnk";

    if ( type == DT_DIR )
        return "dir";

    if ( type == DT_CHR )
        return "ch.dev";

    if ( type == DT_BLK )
        return "block dev";

    if ( type == DT_FIFO )
        return "fifo";

    if ( type == DT_SOCK )
        return "socket";

    if ( type == DT_REG )
        return "reg";

    else
        return  "unkwn";
}
const char* sType ( mode_t type )
{
    if (S_ISLNK(type))
        return "symbolic link";

    if ( S_ISDIR(type) )
        return "directory";

    if ( S_ISCHR(type) )
        return "character device";

    if ( S_ISBLK(type) )
        return "block device";

    if ( S_ISFIFO(type) )
        return "fifo";

    if ( S_ISSOCK(type) )
        return "socket";

    if ( S_ISREG(type) )
        return "regular file";

    else
        return "unknown";
}



struct linux_dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int main(int argc, char* argv[])
{   
    if ( argc != 2 )
    {
        fprintf ( stderr, "Usage: %s dir_path", argv[0] );
        return -1;
    } 

    char *dir_path = argv[1];


    int fd_dir = open ( dir_path, O_RDONLY | O_DIRECTORY );
    if ( fd_dir == -1 )
    {
        perror ("Failed to open ");
        return -1;
    }

    int pos = 0;
    struct linux_dirent *linux_dir;
    struct stat buf_info;
    char buf[BUF_SIZE];
    for ( ; ; )
    {
        int read_bytes = syscall( SYS_getdents, fd_dir, buf, sizeof(char)*BUF_SIZE );

        if ( read_bytes == -1)
        {
            fprintf ( stderr, "syscall getdents error\n" );
            if ( close (fd_dir) == -1 )
                perror ("Failed to close dir");
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

            char dir_type = *(buf + pos + linux_dir->d_reclen - 1); // там хранится байт, отвечающий за тип в структуре
            
            if ( dir_type == DT_UNKNOWN )
            {
                if ( fstatat (fd_dir, linux_dir ->d_name, &buf_info, ASN) == -1 )
                {
                    perror ( "Failed to get stat" );
                    if ( close (fd_dir) == -1 )
                        perror ("Failed to close fd_dir");
                    return -1;
                }
                printf("%-12s ", sType ( buf_info.st_mode ) );
            }
            else
                printf("%-12s ", dType ( dir_type ) );

            printf("%5d %10lld  %s\n", linux_dir->d_reclen, (long long)linux_dir->d_off, linux_dir->d_name);
            pos += linux_dir->d_reclen; // перемещаем указатель на следующую структуру
        }
        
    }

    close (fd_dir);

    return 0;
}
