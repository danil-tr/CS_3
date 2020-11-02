/*
4.

Недостаток utime и utimes: имеют секундную и микросекундную точность
соответственно.

futimens - обновление временных меток файла

futimens не удается обновить метки со всеми правами доступа??

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


const int SIZE = 200;


/*
Создадает файл с возможностью записи и чтения только для владельца.
*/
int fileCreate ( char *file_path )
{
    mode_t mode = S_IRWXU;
    int file_flags = O_WRONLY | O_CREAT ;

    int file_des = open ( file_path, file_flags, mode );

    if ( file_des == -1 )
    {
        fprintf ( stderr, "Не удалось получить файловый дескриптор\n" );
        exit (1);
    }

    return file_des;
}

/*
Создаем структуру и записываем в нее информацию. Находим ошибку если
файл не существует
*/
void fstatCreate(int file_des, struct stat *buf, int line){
    int stat_status = -1;

    stat_status = fstat( file_des, buf );

    if (stat_status == -1)
    {
        fprintf (stderr, "There is no this file in the directory,"
                         "error on line %d\n", line);
        exit (1);
    }
}

char* formatDate ( char* str, time_t val ) 
{
        strftime ( str, 36, "%d.%m.%Y %H:%M:%S", localtime(&val) );
        return str;
}
/*
Вывод информации о файле
*/

void printInfo ( struct stat file_stat, char *buffer, char *file_name )
{
    printf ( "Information for file %s \n", file_name );
    printf ("-------------------------------------\n");
    printf ("File size: %ld bytes \n", file_stat.st_size );
    printf ("Access : %s\n", formatDate ( buffer, file_stat.st_atime ) );
    printf ("Modify : %s\n", formatDate ( buffer, file_stat.st_mtime ) );
    printf ("Владелец: %ld\n", file_stat.st_dev );
    printf ("Время изменения прав : %s\n", formatDate ( buffer, file_stat.st_ctime ) );

} 


int main(int argc, char* argv[])
{   
    char buffer[SIZE];

    struct stat file_stat;

    char *file1_name = argv[1];
    int file1_des = fileCreate ( file1_name );

    //Создадим файл и запишем туда строку
    char *str = "Computer Science"; 

    if ( write( file1_des, str, 17 ) == -1 )
    {
        fprintf ( stderr, "1.Не удалось записать строку в файл\n" );
        exit (1);
    }

    printf ( "Запись строки в файл %s c помощью write прошла успешно.\n", file1_name );
    fstatCreate ( file1_des, &file_stat, __LINE__ );

    printInfo ( file_stat, buffer, file1_name );
    
    //Новые права:
    mode_t new_mode = S_IRWXU | S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP | S_IWGRP | S_IXGRP | S_IRWXO | S_IROTH | S_IWOTH | S_IXOTH;

    if ( fchmod ( file1_des, new_mode ) == -1 )
    {
        fprintf ( stderr, "Не удалось изменить права доступа\n" );
        exit (1);
    }
    printf ( "Права успешно изменены\n" );
    // Обновление временных меток 
    const struct timespec times[2];

    if ( futimens ( file1_des, times ) == -1 )
    {
         fprintf ( stderr, "Не удалось обновить временные метки\n" );
         exit (1);
    }

    close(file1_des);
    return 0;
}
