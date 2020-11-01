/*
3.1 Копирование и чтение инфармации файла 
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

const int SIZE = 20;
const int BUF_SIZE = 100;

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
        fprintf ( stderr, "Не удалось записать строку в файл\n" );
        exit (1);
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
        assert ("fstat file error");
    }

    off_t offset = 0; //считываем file1 сначала;
    if ( sendfile ( file2_des, file1_des, &offset, file_stat.st_size ) == -1 )
    {
        assert ("sendfile error");
    }

    printf ( "Пересылка из файла %s в файл %s прошла успешно.\n", file1_name, file2_name );

    close(file1_des);
    close(file2_des);

    return 0;
}
