/*
7.1 Копирует все файлы из указанной директории в другую
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

#define SIZE (2 << 19) // 1Mb

int copyFile ( const char* src, const char* dst )
{
    uint8_t buffer[PATH_MAX];
    int flag = 0;
    int src_fd = 0, dst_fd = 0;
    src_fd = open ( src, O_RDONLY);
    if ( src_fd == -1 ) 
    {
        perror ( "failed to open src file ");
        flag = -1;
    }

    dst_fd = open ( dst, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR );
    if (dst_fd == -1) 
    {
        perror ( "failed to open dst file" );
        flag = -1;
    }  

    while ( !flag ) 
    {
        ssize_t bytes_read = read ( src_fd, buffer, PATH_MAX );
        if ( bytes_read == -1 ) {
            perror ( "failed to read src file" );
            flag = -1;
            break;
        }

        if ( ( bytes_read == 0 ) || flag ) 
            break;

        ssize_t bytes_written = 0;

        while ( bytes_written < bytes_read ) 
        {
            ssize_t res = write ( dst_fd, buffer + bytes_written, (size_t) ( bytes_read - bytes_written ) );
            if ( res == -1 )  
            {
               perror ( "failed to write dest file" );
               flag = -1;
               break;
            }
    }    }

    close(dst_fd);
    close(src_fd);
    return flag;
}

int copy_dir( char *name, char *dest )
{
    int flag = 0;
    DIR *dir = NULL; 
    struct dirent *entry = NULL;

    if ( (dir = opendir(name)) == NULL ) 
    {
        perror ( "failed to open source file" );
        flag = -1;
    }
    while ( ( !flag && ((entry = readdir(dir)) != NULL) ) ) 
    {
        struct stat stat_buf;

        if ( lstat ( name, &stat_buf ) == -1 )
        {
            perror ( "failed to get stat" );
            flag = -1;
            break;
        }

        if ( S_ISDIR ( stat_buf.st_mode ) ) 
        {
            char path1[PATH_MAX] = {0};
            char dist_path1[PATH_MAX] = {0};
            if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                continue;

            snprintf ( path1, sizeof (path1), "%s/%s", name, entry->d_name );
            snprintf ( dist_path1, sizeof (dist_path1),"%s/%s", dest, entry->d_name );

            mkdir ( dist_path1, 0700 );

            copy_dir ( path1, dist_path1 );
        }
        else if ( S_ISREG ( stat_buf.st_mode ) )
        {
            char path2[PATH_MAX] = {0};
            char dist_path2[PATH_MAX] = {0};

            snprintf ( path2, sizeof (path2), "%s/%s", name, entry->d_name );
            snprintf ( dist_path2, sizeof (dist_path2),"%s/%s", dest, entry->d_name );
            if ( copyFile ( path2, dist_path2 ) == -1)
            {
                fprintf ( stderr, "copy error\n" );
                flag = -1;
                break;
            } 
        } 
    }

    closedir (dir);
    return flag;
}

int main( int argc, char* argv[] )
{
    if ( argc != 3)
    {   
        fprintf ( stderr, "Usage: %s [src_path] [dest_path]\n", argv[0] );
        return -1;
    }

    int res = copy_dir ( argv[1], argv[2] );

    return res;
}