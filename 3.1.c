/*
3.1 Копирование и чтение информации файла 
передаем в консоли названия двух файлов
первый - файл, куда запишем строку
второй - файл, куда скопируем строку из первого


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

    char *file1_name = argv[1];
    char *file2_name = argv[2];


    int file1_des = fileCreate ( file1_name );
    int file2_des = fileCreate ( file2_name );

    //Создадим файл и запишем туда строку
    char str[] = "Computer Science"; 

    if ( write( file1_des, str, 17) == -1 )
    {
        fprintf ( stderr, "Failed to wrtite\n" );
        return -1;
    }

    printf ( "Запись строки в файл %s c помощью write прошла успешно.\n", file1_name );
    close ( file1_des ); //откроем далее только для записи

    file1_des = open ( file1_name, O_RDONLY );
    if ( file1_des == -1 )
    {
        fprintf ( stderr, "Failed to get fd\n" );
        return -1;;
    }
    //Заполним структуру информации о 1 файле (необходим размер первого файла)
    if ( fstat ( file1_des, &file_stat ) == -1 )
    {
        perror ( "fstat file error" );
        return -1;
    }

    int size = file_stat.st_size;
    
    char *buffer = (char *) calloc ( size + 1, sizeof(char) );//массив для записи с файла.

    if ( read( file1_des, buffer, size ) != size )
    {
        assert ( "Impossible to read" );
    }
    if ( write( file2_des, buffer, size) == -1 )
    {
        fprintf ( stderr, "2.Не удалось записать строку в файл\n" );
        exit (1);
    }

    printf ( "Пересылка из файла %s в файл %s прошла успешно.\n", file1_name, file2_name );

    /*
    Продемонстрируем, что read и write смещают указатели при файловой обработке:
    при повторной записи в file2 символы запишутся в конец
    при повторного считывания из file1 начнут считываться новые символы
    (например, если их там нет, то на кол-во считанных укажет возвращаемое значение read)
    */

    char *new_string = "abcdefg";
    if ( write( file2_des, new_string, 9) == -1 )
    {
        fprintf ( stderr, "2.Не удалось записать строку в файл\n" );
        return -1;
    }
    //Попробуем что-то считать с file1
    char test_buffer[BUF_SIZE];
    int sym_count = read ( file1_des, test_buffer, 5 );
    printf ( "Количество считанных символов read после основного прочтения: %d ", sym_count);//получили 0 чтд


    


    close(file1_des);
    close(file2_des);

    free(buffer);
    return 0;
}
