/*
System Calls: stat, fstat,

в argv[1] помещаем название файла в текущей директории
данная программа выведет информацию о файлах
Пример: ./task task.c выведет информацию о данном файле.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

char*  sType ( mode_t type )
{
    mode_t file_type = type & S_IFMT;
    if ( file_type == S_IFLNK ) 
        return "s.lnk";

    else if ( file_type == S_IFDIR )
        return "dir";

    else if ( file_type == S_IFCHR )
        return "ch.dev";

    else if ( file_type == S_IFBLK )
        return "block dev";

    else if ( file_type == S_IFIFO )
        return "fifo";

    else if ( file_type == S_IFSOCK )
        return "socket";

    else if ( file_type == S_IFREG )
        return "reg";

    else
        return  "unkwn";
}
int main(int argc, char* argv[])
{   
    if ( argc != 2 )
    {
        printf ( "Usage %s [file]", argv[0] );
        return -1;
    }

    const char *file_path = argv[1];
    struct stat buf_info;

    if ( lstat( file_path, &buf_info ) == -1 )
    {
        perror ( "Failed to get stat" );
        return -1;
    }

    printf("Information for file %s \n\n", file_path);
    printf("File size: %ld bytes \n", buf_info.st_size);
    printf("Number of links: %lu \n", buf_info.st_nlink);
    printf("File inode: %lu \n", buf_info.st_ino);
    printf("Type: %s\n", sType ( buf_info.st_mode ));

    return 0;
}
