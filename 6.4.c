
/*
6.4
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
#include <errno.h>

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

char*  sType ( mode_t type )
{
    mode_t file_type = type & S_IFMT;
    if ( file_type == S_IFLNK ) 
        return "s.lnk";

    else if ( file_type == S_IFDIR )
        return "dir";

    else if ( file_type == S_IFCHR )
        return "ch.dev";

    else if ( file_type == S_IFBLK )
        return "block dev";

    else if ( file_type == S_IFIFO )
        return "fifo";

    else if ( file_type == S_IFSOCK )
        return "socket";

    else if ( file_type == S_IFREG )
        return "reg";

    else
        return  "unkwn";
}




int view_dir( const char *name, int level )
{
    int flag = 0;
    DIR *dir = NULL; 
    struct dirent *entry = NULL;
    struct stat buf_info;

    if ( (dir = opendir(name)) == NULL ) 
        {
            perror ( "Failed to open dir." );
            flag = -1;
        }

    int fd_dir = dirfd ( dir );
    if ( fd_dir == -1 )
    {
        perror ( "Failed to get fd_dir" );
        flag = -1;
    }

    while ( ((entry = readdir(dir)) != NULL ) && !flag ) 
    {   
        if ( entry->d_type == DT_UNKNOWN )
        {
            if ( fstatat( fd_dir, entry -> d_name, &buf_info, AT_SYMLINK_NOFOLLOW ) == -1 )//вернуть информацию о символьной ссылки, а не разыменовать ее
            {
                perror("Failed to get stat");
                return -1;
            }

            if ( S_ISDIR ( buf_info.st_mode ) )
            {
                char path[PATH_MAX] = {0};
                if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                    continue;
                snprintf (path, sizeof(path), "%s/%s", name, entry->d_name);
                printf ("%*s[%s]\n", level, "", entry->d_name); 
                view_dir ( path, level + 4 );
            }
            else
            {
                 printf("%*s| %s (%s)\n", level, "", entry->d_name, sType (buf_info.st_mode )); 
            }
        }
        else  if ( entry->d_type == DT_DIR ) 
        {
            char path[PATH_MAX] = {0};
            if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                continue;
            snprintf (path, sizeof(path), "%s/%s", name, entry->d_name);
            printf ("%*s[%s]\n", level, "", entry->d_name); 
            view_dir ( path, level + 4 );
        }
        else
            printf("%*s| %s (%s)\n", level, "", entry->d_name, dType ( entry->d_type ));   
    }

    close ( fd_dir );
    closedir ( dir );
    return flag;
}

int main( int argc, char* argv[] )
{

    if ( argc != 2 ) 
    {  
        fprintf ( stderr, "Usage: %s [path]\n", argv[0] );
        return -1;
    }

    const char *path = argv[1];

    int res = view_dir ( path, 0);
    
    return res;
}   
