/*
9 Вывод информации о дисковом пространстве
*/

#include <dirent.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const double GB_size = 1073741824;

const char* checkConsole (int argc_value, char* argv_value)
{
    char *path = NULL;

    if ( argc_value == 2 )
    {
        path = argv_value;
    } 
    else if ( argc_value == 1 )
    {
        return "/";
    }
    else
    {
        fprintf ( stderr, "argc error" );
        exit (1);
    }

    size_t len_path = strlen ( path );

    if ( path[len_path - 1] != '/' )
    {
        path[len_path] = '/';
        path[len_path + 1] = '\0';
    }
    return path;
}

int main( int argc, char* argv[] )
{
    const char* path = checkConsole ( argc, argv[1] );
    struct statvfs buf;

    if ( statvfs ( path, &buf ) == -1 )
    {
        printf ( "%s\n", strerror(errno) );
        exit(1);
    }
    unsigned long frag_size = buf.f_frsize;
    unsigned long fsys_size = frag_size * ( buf.f_blocks );
    unsigned long avl_size = frag_size * ( buf.f_bfree );
    
    printf ("---Disk capacity information---\n");
    printf ("all space: %lf GB\n", fsys_size/GB_size );
    printf ("available space: %lf GB\n", avl_size/GB_size );
    printf ("used space: %lf GB\n", (fsys_size - avl_size)/GB_size );

    return 0;
}