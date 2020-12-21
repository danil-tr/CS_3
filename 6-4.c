
/*
6.4

переделать: рекурсивный обход с сохранением 
большого кол-ва строк
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



int view_dir( const char *name, int level )
{
    DIR *dir = NULL; 
    struct dirent *entry = NULL;
    struct stat buf_info;

    if ( (dir = opendir(name)) == NULL ) 
        {
            perror ( "Failed to open dir." );
            return -1;
        }

    while ( (entry = readdir(dir)) != NULL ) 
    {   
        if ( entry->d_type == DT_UNKNOWN )
        {   
            char path[PATH_MAX] = {0};
            snprintf (path, sizeof(path), "%s/%s", name, entry->d_name);

            if ( lstat(path, &buf_info) == -1 )
            {
                if ( closedir ( dir ) == -1 )
                    perror ("Failed to close dir");
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

    closedir ( dir );
    return 0;
}

int main ( int argc, char* argv[] )
{

    if ( argc != 2 ) 
    {  
        fprintf ( stderr, "Usage: %s [path]\n", argv[0] );
        return -1;
    }

    const char *path = argv[1];

    if ( view_dir ( path, 0) )
    {
        fprintf ( stderr, "Failed to view dir" );
        return -1;
    }
    
    return 0;
}   
