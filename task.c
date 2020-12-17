/*
7.1 Копирует все файлы из указанной директории в другую
*/

#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 2056

void copyFile ( const char* src, const char* dst )
{
    char *buffer;
    int src_fd, dst_fd, n, err;
    const size_t BUF_SIZE = 1000000000;

    buffer = (char *) calloc (BUF_SIZE, sizeof(char));
    mode_t mode = S_IRWXU | S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRGRP | 
                  S_IWGRP | S_IXGRP | S_IRWXO | S_IROTH | S_IWOTH | S_IXOTH;

    src_fd = open(src, O_RDWR, mode);
    if (src_fd == -1) {
            printf("Error src_fd.\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
    dst_fd = open(dst, O_CREAT | O_RDWR | O_TRUNC , mode);
    if (dst_fd == -1) {
            printf("Error dst_fd.\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
    while (1) {
        err = read(src_fd, buffer, BUF_SIZE);
        if (err == -1) {
            printf("Error reading file.\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
        n = err;

        if (n == 0) 
            break;

        err = write(dst_fd, buffer, n);
        if (err == -1) {
            printf("%s\n", strerror(errno));
            printf("Error writing to file.\n");
            exit(1);
        }
    }

    free (buffer);
    close(src_fd);
    close(dst_fd);
}

void copy_dir( const char *name, const char *dist )
{
    DIR *dir = NULL; //объявляем в функции (не в глобалных пременных), чтобы при рекурсивном вызове обновлялись
    struct dirent *entry = NULL;

    if ( (dir = opendir(name)) == NULL ) // при ошибке открытия файла выдаст NULL указатель
        assert ( "opendir error" );

    while ( (entry = readdir(dir)) != NULL ) 
    {
        if (entry->d_type == DT_DIR) 
        {
            char path1[SIZE] = {0};
            char dist_path1[SIZE] = {0};
            if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                continue;

            snprintf ( path1, sizeof (path1), "%s/%s", name, entry->d_name );
            snprintf ( dist_path1, sizeof (dist_path1),"%s/%s", dist, entry->d_name );

            mkdir ( dist_path1, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

            copy_dir ( path1, dist_path1 );
        }
        else
        {
            char path2[SIZE] = {0};
            char dist_path2[SIZE] = {0};
            snprintf ( path2, sizeof (path2), "%s/%s", name, entry->d_name );
            snprintf ( dist_path2, sizeof (dist_path2),"%s/%s", dist, entry->d_name );
            copyFile ( path2, dist_path2 ); 
        } 
    }
    closedir(dir);
}

int main( int argc, char* argv[] )
 {
    if ( argc != 3)
        return -1;

    const char* dir_path = argv[1];
    const char* dist_path = argv[2];

    copy_dir ( dir_path, dist_path );

    return 0;
}