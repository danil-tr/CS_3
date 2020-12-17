//6.4 Рекурсивный просмотр каталогов

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

char* checkType ( unsigned char type )
{
if ( type == DT_LNK ) 
return "s.lnk";

else if ( type == DT_DIR )
return "dir";

else if ( type == DT_CHR )
return "ch.dev";

else if ( type == DT_BLK )
return "block dev";

else if ( type == DT_FIFO )
return "fifo";

else if ( type == DT_SOCK )
return "socket";

else if ( type == DT_REG )
return "reg";

else
return  "unkwn";
}


char* checkConsole (int argc_value, char* argv_value)
{
    char *path = NULL;

    if ( argc_value == 2 )
    {
        path = argv_value;
    } 
    else if ( argc_value == 1 )
    {
        return "./";
    }
    else
    {
        fprintf ( stderr, "argc error" );
        exit (1);
    }

    size_t len_path = strlen ( path );

    if ( path[len_path - 1] != '/' )
    {
        path[len_path] = '/';
        path[len_path + 1] = '\0';
    }
    return path;
}


void view_dir( const char *name, int level )
{
    DIR *dir; //объявляем в функции (не в глобалных пременных), чтобы при рекурсивном вызове обновлялись
    struct dirent *entry;

    if ( (dir = opendir(name)) == NULL ) // при ошибке открытия файла выдаст NULL указатель
        assert ( "opendir error" );

    while ( (entry = readdir(dir)) != NULL ) 
    {
        if (entry->d_type == DT_DIR) 
        {
            char path[2056];
            if (strcmp ( entry->d_name, "." ) == 0 || strcmp ( entry->d_name, ".." ) == 0)
                continue;

            snprintf (path, sizeof(path), "%s/%s", name, entry->d_name);
            printf ("%*s[%s]\n", level, "", entry->d_name); //динамическое указание ширины вывода

            view_dir ( path, level + 4 );
        }
        else
            printf("%*s| %s (%s)\n", level, "", entry->d_name, checkType ( entry->d_type ));   
    }
    closedir(dir);
}

int main( int argc, char* argv[] )
 {
    const char *dir_path = checkConsole ( argc, argv[1] );
    
    view_dir ( dir_path, 0);
    
    return 0;
}
