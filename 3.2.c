/*
3.2 Продемонстрируем отличия pread и pwrite:
Pread() works just like read() but reads from the specified position
in the file without modifying the file pointer.
Pwrite() не изменяется позицию указателя в файле при записи.
+ возвращают кол-во записанных/считанных байт, а не символов.
(в нашем случае не измениттся, тк sizeof(char) = 1)
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

const int BUF_SIZE = 6;

/*
Создадает файл с возможностью записи.
*/
int fileCreate ( char *file_path )
{
    mode_t mode = S_IRUSR | S_IWUSR| S_IRGRP | S_IWGRP | S_IROTH;
    int file_flags = O_WRONLY | O_CREAT;

    int file_des = open ( file_path, file_flags, mode );

    if ( file_des == -1 )
    {
        fprintf ( stderr, "Не удалось получить файловый дескриптор\n" );
        exit (1);
    }

    return file_des;
}


int main(int argc, char* argv[])
{   
    struct stat file_stat;

    off_t offset = 0; //смещение равно 0 для pwrite pread

    char *file1_name = argv[1];
    char *file2_name = argv[2];


    int file1_des = fileCreate ( file1_name );
    int file2_des = fileCreate ( file2_name );

    //Создадим файл и запишем туда строку
    char str[] = "Computer Science"; 

    if ( pwrite( file1_des, str, 17*sizeof(char), offset) == -1 )
    {
        fprintf ( stderr, "1.Не удалось записать строку в файл\n" );
        return -1;
    }

    printf ( "Запись строки в файл %s c помощью write прошла успешно.\n", file1_name );
    close ( file1_des ); //откроем далее только для записи

    file1_des = open ( file1_name, O_RDONLY );
    if ( file1_des == -1 )
    {
        fprintf ( stderr, "Не удалось получить файловый дескриптор\n" );
        exit (1);
    }
    //Заполним структуру информации о 1 файле (необходим размер первого файла)
    if ( fstat ( file1_des, &file_stat ) == -1 )
    {
        perror ("Failed to get stat ");
        return -1
    }

    int size = file_stat.st_size;
    
    char *buffer = (char *) calloc ( size + 1, sizeof(char) );//массив для записи с файла.

    if ( pread( file1_des, buffer, size*sizeof(char), offset ) != size )
    {
        fprintf ( stderr, "Impossible to read" );
    }
    if ( pwrite( file2_des, buffer, size*sizeof(char), offset ) == -1 )
    {
        fprintf ( stderr, "2.Не удалось записать строку в файл\n" );
        exit (1);
    }

    printf ( "Пересылка из файла %s в файл %s прошла успешно.\n", file1_name, file2_name );

    /*
    Продемонстрируем, что pread и pwrite смещают указатели при файловой обработке:
    read может считать с начала файла все символы
    write запишет строку в начало файла (с заменой)
    */

    char *new_string = "abcdefg"; 
    if ( write( file2_des, new_string, 9) == -1 )
    {
        fprintf ( stderr, "2.Не удалось записать строку в файл\n" );
        exit (1);
    }
    //Попробуем что-то считать с file1
    char test_buffer[BUF_SIZE];
    int sym_count = read ( file1_des, test_buffer, size );
    printf ( "Количество считанных символов read после основного прочтения: %d, всего в файле было: %d;\n", sym_count, size);//получили sym_count = size чтд


    


    close(file1_des);
    close(file2_des);

    free(buffer);
    return 0;
}
