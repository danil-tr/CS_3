
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

#define SIZE (2 << 25) // 1Mb

int copyFile ( const char* src, const char* dst )
{
    uint8_t *buffer[SIZE];

    int src_fd = open (src, O_RDONLY);
    if (src_fd == -1) 
    {
        perror ( "Failed to get src_fd." );
        return -1;
    }

    int dst_fd = open ( dst, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR );
    if (dst_fd == -1) 
    {
        perror ( "Failed to get src_fd." );
        if ( close(src_fd) == -1 )
            perror ( "Failed to close src_fd" );
        return -1;
    }
    while (1) 
    {
        ssize_t bytes_read = read ( src_fd, buffer, SIZE );
        if ( bytes_read == -1 ) {
            perror ( "failed to read src file" );
            close (src_fd);
            close (dst_fd);
            return -1;
        }

        if ( bytes_read == 0 ) 
            break;

        ssize_t bytes_written = 0;

        while ( bytes_written < bytes_read ) 
        {
            ssize_t res = write ( dst_fd, buffer + bytes_written, (size_t) ( bytes_read - bytes_written ) );
            if ( res == -1 )  
            {
                perror ( "failed to write dest file" );
                close (src_fd);
                close (dst_fd);
                return -1;
            }
    }    }

    close(dst_fd);
    close(src_fd);

    return 0;
}

int copy_dir( const char *name, const char *dest )
{
    DIR *dir = NULL; //объявляем в функции (не в глобалных пременных), чтобы при рекурсивном вызове обновлялись
    struct dirent *entry = NULL;

    if ( (dir = opendir(name)) == NULL ) // при ошибке открытия файла выдаст NULL указатель
    {
        perror ( "Failed to open dir." );
        return -1;
    }

    while ( (entry = readdir(dir)) != NULL ) 
    {   
        struct stat buf_info;

        if ( entry->d_type == DT_UNKNOWN )
        {   
            char path[10*PATH_MAX];
            snprintf (path, sizeof(path), "%s/%s", name, entry->d_name);

            if ( lstat(path, &buf_info) == -1 )
            {
                if ( closedir ( dir ) == -1 )
                    perror ("Failed to close dir");
                return -1;
            }

            if ( S_ISDIR ( buf_info.st_mode ) )
            {
                char path1[10*PATH_MAX];
                char dest_path1[10*PATH_MAX];
                if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                    continue;

                snprintf ( path1, sizeof (path1), "%s/%s", name, entry->d_name );
                snprintf ( dest_path1, sizeof (dest_path1),"%s/%s", dest, entry->d_name );

                mkdir ( dest_path1, 0700 );

                copy_dir ( path1, dest_path1 );
            }
            else
            {
                char path2[10*PATH_MAX] = {0};
                char dest_path2[10*PATH_MAX] = {0};

                snprintf ( path2, sizeof (path2), "%s/%s", name, entry->d_name );
                snprintf ( dest_path2, sizeof (dest_path2),"%s/%s", dest, entry->d_name );

                copyFile ( path2, dest_path2 ); 
            }
        }
        else  if ( entry->d_type == DT_DIR ) 
        {
            char path2[10*PATH_MAX];
            char dest_path2[10*PATH_MAX];

            snprintf ( path2, sizeof (path2), "%s/%s", name, entry->d_name );
            snprintf ( dest_path2, sizeof (dest_path2),"%s/%s", dest, entry->d_name );

            mkdir ( dest_path2, 0700 );
            copy_dir ( path2, dest_path2 );
        }
        else
        {
                char path2[10*PATH_MAX];
                char dest_path2[10*PATH_MAX];

                snprintf ( path2, sizeof (path2), "%s/%s", name, entry->d_name );
                snprintf ( dest_path2, sizeof (dest_path2),"%s/%s", dest, entry->d_name );

                copyFile ( path2, dest_path2 );
        }
    }

    closedir ( dir );
    return 0;
}

int main( int argc, char* argv[] )
 {
    if ( argc != 3)
    {
        fprintf ( stderr, "Usage: %s src_dir dest_dir\n", argv[0] );
        return -1;
    }
    const char* dir_path = argv[1];
    const char* dest_path = argv[2];

    copy_dir ( dir_path, dest_path );

    return 0;
}