/*
System Calls: stat, fstat, statx

в argv[1] помещаем название файла в текущей директории
данная программа выведет информацию о файлах
Пример: ./task task.c выведет информацию о данном файле.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>

const int ERR_NO = 1;

/*
Вводим строго два аргумента в консоли
*/
void checkArgc(int a){
    assert (a == 2);
}

/*
Создаем структуру и записываем в нее информацию. Находим ошибку если
файл не существует
*/
int statCreate(const char* file_path, struct stat *buf, int line){
    int flag = 0;

    flag = stat( file_path, buf );

    if (flag == -1)
    {
        fprintf (stderr, "There is no this file in the directory,"
                         "error on line %d\n", line);
        return -1;
    }
    else
        return ERR_NO;

}

int main(int argc, char* argv[])
{   
    checkArgc(argc);
    
    const char* file_path = argv[1]; //указатель на имя файла из командной строки

    struct stat file_stat;//структура, где храним данныео файле
    if(statCreate(file_path, &file_stat, __LINE__) != ERR_NO)
        return -1;


    printf("Information for file %s \n\n", file_path);
    printf("File size: %ld bytes \n", file_stat.st_size);
    printf("Number of links: %lu \n", file_stat.st_nlink);
    printf("File inode: %lu \n", file_stat.st_ino);
    printf("The file %s a symbolic link\n", (S_ISLNK(file_stat.st_mode)) ? "is" : "is not");



    return 0;
}
