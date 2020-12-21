/*
3.2
*/

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const int buf_size = (2 << 19);

int main ( int argc, char* argv[] )
{   
    if ( argc != 3 )
    {
        fprintf( stderr, "Usage: %s src_file dest_file\n", argv[0] );
        return 1;
    }   

    char *src_path = argv[1];
    char *dest_path = argv[2];

    struct stat buf_info;

    if ( lstat (src_path, &buf_info) == -1 )
    {
        perror ( "Failed to get lstat" );
        return -1;
    }
    if ( !S_ISREG (buf_info.st_mode) )
    {
        perror ( "Error: choose regular file" );
        return -1;
    }
    
    int src_des = open ( src_path, O_RDONLY );
    if ( src_des == -1 )
    {
        perror ( "Failed to get source des" );
        return -1;
    }

    uint8_t buffer[buf_size];//запишем файл в буфер
    ssize_t bytes_read = 0;
    while ( bytes_read < ((ssize_t) buf_size) ) 
    {
        ssize_t res = pread ( src_des, buffer, (size_t) buf_size, (off_t) bytes_read );
        if ( res == 0 )
        {
            if ( fsync ( src_des ) == -1 )
            {
                perror ("Failed to save data");
                return -1;
            }

            if ( close(src_des) == -1 )
            {
                perror ( "Failed to close src des" );
                return -1;
            }
            break;
        }
        if ( res == -1 )  
        {
            perror ( "Failed to use read()" );
            if ( close (src_des) == -1 )
            {
                perror ("Failed to close fd");
                return -1;
            }
        }
        bytes_read += res;
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

    int dest_des = open ( dest_path, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, mode );
    if ( dest_des == -1 )
    {
        perror ( "Failed to get fd with open()" );
        return -1;
    }

    ssize_t bytes_written = 0;
    while ( bytes_written < bytes_read ) 
    {
        ssize_t res = pwrite ( dest_des, buffer, (size_t) bytes_read, (off_t) bytes_written);
        if ( res == -1 )  
        {
            perror ( "Failed to use write()" );
            if ( close ( dest_des ) == -1 )
                perror ("Failed to close fd");
            return -1; 
        }
        bytes_written += res;
    }

    if ( fsync ( dest_des ) == -1 )
    {
        perror ("Failed to save data");
        return -1;
    }

    if ( close (dest_des ) == -1 )
    {
        perror ( "Failed to close fd" );
        return -1;
    }

    
    /*
    Продемонстрируем, что pread и pwrite смещают указатели при файловой обработке:
    read может считать с начала файла все символы
    write запишет строку в начало файла (с заменой)
    

    char *new_string = "abcdefg"; 
    if ( write( file2_des, new_string, 9) == -1 )
    {
        fprintf ( stderr, "2.Не удалось записать строку в файл\n" );
        exit (1);
    }
    //Попробуем что-то считать с file1
    char test_buffer[BUF_SIZE];
    int sym_count = read ( file1_des, test_buffer, size );
    printf ( "Количество считанных символов read после основного прочтения: %d, всего в файле было: %d;\n", sym_count, size);
    получили sym_count = size чтд
    */

    return 0;
}
