
/*
6.1 Вывод содержимого каталога.

путь к каталогу в виде ./aa/bbb/
аргумент строки - имя каталога
*/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

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


int main(int argc, char* argv[])
{   

    if ( argc != 2 )
    {
        fprintf ( stderr, "Usage: %s dir", argv[0] );
        return -1;
    }

    char *dir_path = argv[1];
    
    size_t len_path = strlen ( dir_path );
    
    if ( dir_path[len_path - 1] != '/' )
    {
        dir_path[len_path] = '/';
        dir_path[len_path + 1] = '\0';
        len_path++;
    }


    DIR *dir = opendir ( dir_path );
    if ( dir == NULL )
    {
        perror ( "Failed to open dir" );
        return -1;
    }

    int fd_dir = dirfd ( dir );
    if ( fd_dir == -1 )
    {
        perror ( "Failed to get dirfd" );
        closedir (dir);
        return -1;
    }

    struct dirent *entry; //указатель на структуру, которая хранит данные после readdir
    struct stat buf_info;

    while ( ( entry = readdir (dir) ) != NULL )
    {
        char *dir_part = entry->d_name;

        if ( entry->d_type == DT_UNKNOWN)
        {
            if ( fstatat(fd_dir, dir_part ,&buf_info, AT_SYMLINK_NOFOLLOW) == -1 ) // не разыменовываем ссылки
            {
                perror ("Failed to get stat");
                if ( close (fd_dir) == -1)
                    perror ("Failed to close fddir");
                if ( closedir ( dir ) == -1 )
                    perror ("Failed to close dir");
                return -1;
            }
            printf ( "%-18s: %s\n", sType(buf_info.st_mode), dir_part );
        }
        else      
            printf ( "%-18s: %s\n", dType(entry->d_type), dir_part );
        
    }

    close ( fd_dir );
    closedir ( dir );
    
    return 0;
}
