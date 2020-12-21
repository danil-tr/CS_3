/*
4.

Недостаток utime и utimes: имеют секундную и микросекундную точность
соответственно.

futimens - обновление временных меток файла

utimens:
Если pathname указывает на символьную ссылку, 
то обновляются временные метки ссылки, а не файла, на который она ссылается.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


const int SIZE = (2<<9);

char* formatDate ( char* str, time_t val ) 
{
        strftime ( str, 36, "%d.%m.%Y %H:%M:%S", localtime(&val) );
        return str;
}
/*
Вывод информации о файле
*/

int main(int argc, char* argv[])
{   
    if (argc != 2)
    {
        fprintf ( stderr, "Usage: %s file_name\n", argv[0] );
        return 1;
    }   

    char *path = argv[1];

    int file_des = open ( path, O_RDONLY | O_CREAT, 0600 );

    if ( file_des == -1 )
    {
        perror ( "Failed to get file_des" );
        return -1;
    }

    struct stat buf_info;

    if ( fstat ( file_des, &buf_info ) == -1 )
    {
        perror ( "Failed to get fstat" );
        if ( close(file_des) == -1 )
            perror ( "Failed to close des" );
        return -1;
    }

    mode_t new_mode = S_IRWXU | S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP;

    if ( fchmod ( file_des, new_mode ) == -1 )
    {
        perror ( "Failed to change mode" );
        if ( close(file_des) == -1 )
            perror ( "Failed to close des" );
        return -1;
    }

    // Обновление временных меток 
    const struct timespec times[2];

    if ( futimens ( file_des, times ) == -1 )
    {
        perror ( "Failed futimens " );
        if ( close(file_des) == -1 )
            perror ( "Failed to close des" );
        return -1;
    }

    char buffer1[SIZE];
    char buffer2[SIZE];
    printf ("atime: %s", formatDate (buffer1, times->tv_sec) );
    printf ("mtime: %s", formatDate (buffer2, times->tv_sec) );
    close(file_des);

    return 0;
}
