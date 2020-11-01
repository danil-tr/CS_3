/*
Работа с файлами: Запись в файл строку с клавиатуры.
Создание двух файлов и работа уже с существующим
argv[1] передаем имя файла
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const int SIZE = 1000;

/*
Функция для считывания строки с клавиатуры. Вовзращает указатель на строку.
*/

char* Input(int *length)
{
    int MAX_SIZE = 10;
    int cur_length = 0;
    *length = 0;

    char* buffer = (char *) calloc(MAX_SIZE, sizeof(char));

    while ( scanf("%10[^\n]%n", buffer + *length, &cur_length) != 0 )
    {   
        scanf ("%*1[\n]");
        *length += cur_length;

        if ( *length > MAX_SIZE/2 )
        {
            MAX_SIZE *= 2;

            buffer = (char*) realloc(buffer, MAX_SIZE * sizeof(char) );
        }
    }
    *(buffer + *length) = '\0';

    return buffer;
}

/*
Вводим строго два аргумента в консоли
*/
void checkArgc(int a){
    assert (a == 2);
}


int main(int argc, char* argv[])
{   
    checkArgc(argc);

    /*Блок работы с консоль*/
    int string_length = 0;
    char *str = NULL;

    printf("Enter the string that you want to write to the file...\n");
    str = Input(&string_length);
    printf("String: %s, length: %d\n", str, string_length);
    
    /*Блок работы с файлами*/

    //Путевое имя файла
    char *file_path = argv[1];

    
    mode_t mode = S_IRUSR | S_IWUSR| S_IRGRP | S_IWGRP | S_IROTH;//доступ на чтение и запись
    /*
    O_TRUNC приводит к очистке существующего файла. Данные, записываемые в файл,
     замещают предыдущее содержимое файла.
    O_APPEND приводит к открытию файла в режиме добавления. Данные, записываемые в
    файл, добавляются в его конец.
    O_CREAT означает создание нового файла. Если указанное имя соответствует
    несуществующему файлу, он будет создан.
    */
    int file_flags = O_WRONLY | O_CREAT | O_APPEND;

    int file_des = open ( file_path, file_flags, mode );
    if ( file_des == -1 )
    {
        fprintf(stderr, "Не удалось получить файловый дескриптор\n");
        exit(1);
    }
    if( write(file_des, str, string_length) == -1 )
    {
        fprintf(stderr, "Не удалось записать строку в файл\n");
        exit(1);
    }
    printf("Запись в файл %s c помощью write прошла успешно.\n", argv[1]);

    const char f_string[] = "dprintf as write\n";

    printf("\n*Запись с помощью dprintf строки: %s*\n", f_string);
    dprintf(file_des, f_string);

    close(file_des);
    free(str);

    return 0;
}
