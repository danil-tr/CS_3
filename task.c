/*
6.1 Вывод содержимого каталога.

путь к каталогу в виде ./aa/bbb/
аргумент строки - имя каталога
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

const char* File_type ( int fd_dir, const char* path )
{
struct stat st;
fstatat ( fd_dir, path, &st, 0 );
if (S_ISLNK(st.st_mode))
return "symbolic link";

else if ( S_ISDIR(st.st_mode) )
return "directory";

else if ( S_ISCHR(st.st_mode) )
return "character device";

else if ( S_ISBLK(st.st_mode) )
return "block device";

else if ( S_ISFIFO(st.st_mode) )
return "fifo";

else if ( S_ISSOCK(st.st_mode) )
return "socket";

else if ( S_ISREG(st.st_mode) )
return "regular file";

else
assert ( "unknown file's type" );

}



int main(int argc, char* argv[])
{   
    char entry_path[PATH_MAX - 1] = {0};
    char *dir_path;
    struct dirent *entry;
    
    if ( argc == 2 )
    {
        dir_path = argv[1];
    } 
    else if ( argc == 1 )
    {
        dir_path = ".";
    }
    else
    {
        fprintf ( stderr, "argc error" );
        exit (1);
    }
    
    size_t len_path = strlen ( dir_path );
    strncpy ( entry_path, dir_path, len_path );
    
    if ( entry_path[len_path - 1] != '/' )
    {
        entry_path[len_path] = '/';
        entry_path[len_path + 1] = '\0';
        len_path++;
    }

    DIR *dir = opendir ( dir_path );
    assert ( dir != NULL );

    int fd_dir = dirfd ( dir );
    assert ( fd_dir != -1 );

    const char clear_part[PATH_MAX - 1] = {0};

     while ( ( entry = readdir (dir) ) != NULL )
    {
        const char *type;

        char *dir_part = entry->d_name;
        size_t dir_part_len = strlen ( dir_part );

        strncpy ( entry_path + len_path, dir_part, dir_part_len );

        type = File_type ( fd_dir, entry_path );
        printf ( "%-18s: %s\n", type, entry_path );

        strncpy ( entry_path + len_path, clear_part, dir_part_len );
        
    }   


    close( fd_dir );		
    closedir( dir );

    return 0;
}
