/*
2.1 

dprintf = fprintf 
These functions are GNU extensions that are nowadays specified in POSIX
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main ( int argc, char* argv[] )
{   
    if ( argc != 3 )
    {
        fprintf ( stderr, "Usage: %s file_name string\n", argv[0]);
        return -1;
    }
    
    const char *filepath = argv[1];
    const char *str = argv[2]; 

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

    int file_des = open ( filepath, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, mode );
    if ( file_des == -1 )
    {
        perror ( "Failed to get fd with open()" );
        return -1;
    }

    ssize_t bytes_written = 0;
    ssize_t bytes_read = strlen ( str ) ; // записываем с'\0'
    while ( bytes_written < bytes_read ) 
    {
        ssize_t res = write ( file_des, str + bytes_written, (size_t) ( bytes_read - bytes_written ) );
        if ( res == -1 )  
        {
            perror ( "Failed to use write()" );
            if ( close ( file_des ) == -1 )
                perror ("Failed to close fd");
            return -1; 
        }
        bytes_written += res;
    }

    if ( fsync ( file_des ) == -1 )
    {
        perror ("Failed to save data");
        return -1;
    }

    if ( close(file_des) == -1 )
    {
        perror ( "Failed to close fd" );
        return -1;
    }

    return 0;
}
