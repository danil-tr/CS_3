
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
#include <limits.h>

const char* sType ( mode_t type )
{
    if (S_ISLNK(type))
        return "symbolic link";

    else if ( S_ISDIR(type) )
        return "directory";

    else if ( S_ISCHR(type) )
        return "character device";

    else if ( S_ISBLK(type) )
        return "block device";

    else if ( S_ISFIFO(type) )
        return "fifo";

    else if ( S_ISSOCK(type) )
        return "socket";

    else if ( S_ISREG(type) )
        return "regular file";

    else
        return "unknown";
}

char* dType ( unsigned char type )
{
    if ( type == DT_LNK ) 
        return "s.lnk";

    else if ( type == DT_DIR )
        return "dir";

    else if ( type == DT_CHR )
        return "ch.dev";

    else if ( type == DT_BLK )
        return "block dev";

    else if ( type == DT_FIFO )
        return "fifo";

    else if ( type == DT_SOCK )
        return "socket";

    else if ( type == DT_REG )
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

    struct dirent *entry; //указатель на структуру, которая хранит данные после readdir
    struct stat buf_info;
    const char clear_part[PATH_MAX] = {0};

    while ( ( entry = readdir (dir) ) != NULL )
    {
        char *dir_part = entry->d_name;
        size_t dir_part_len = strlen ( dir_part );

        strncpy ( dir_path + len_path, dir_part, dir_part_len + 1 );

        if ( entry->d_type == DT_UNKNOWN)
        {
            if ( lstat(dir_path, &buf_info) == -1 )
            {
                perror ("Failed to get stat");
                if ( closedir ( dir ) == -1 )
                    perror ("Failed to close dir");
                return -1;
            }
            printf ( "%-18s: %s\n", sType(buf_info.st_mode), dir_path );
        }
        else
        {        
            printf ( "%-18s: %s\n", dType(entry->d_type), dir_path );
        }

        strncpy ( dir_path + len_path, clear_part, dir_part_len + 1 );
    }

    closedir ( dir );
    
    return 0;
}
