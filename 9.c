/*
9 Вывод информации о дисковом пространстве

frsize is the "fundamental block size": the size of the smallest independently
addressable/allocatable data block on the storage.

bsize is the "preferred block size": the system will attempt to do all operations 
in multiples of this size unless it absolutely must do otherwise.

statfs() является OS-специфичным
statvfs() - posix-соответствие

*/


#include <stdio.h>
#include <sys/statvfs.h>
#include <string.h>
#include <stdlib.h>

const double GB_size = 2 << 29;

int main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        fprintf ( stderr, "Usage: %s [path]\n", argv[0] );
        return -1;
    }

    const char *path = argv[1];
    struct statvfs buf;

    if ( statvfs ( path, &buf ) == -1 )
    {
        perror ( "statvfs error on line\n");
        exit(1);
    }
    unsigned long frag_size = buf.f_frsize;
    unsigned long fsys_size = frag_size * ( buf.f_blocks );
    unsigned long avl_size = frag_size * ( buf.f_bavail ); //что доступно пользователю
    unsigned long all_avl_size = frag_size * ( buf.f_bfree ); //все место - доступное системе
    unsigned long used_size = fsys_size - all_avl_size;
    
    printf ("---Disk capacity information---\n");
    printf ("all space: %lf GB\n", fsys_size/GB_size );
    printf ("available space: %lf GB\n", avl_size/GB_size );
    printf ("used space: %lf GB\n", used_size/GB_size );

    return 0;
}