/*
10. Отслеживание новых файлов в указанном каталоге.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

const int buf_size =  ( 2 << 19 );

int main(int argc, char *argv[]) 
{
	if (argc != 2) 
    {
		fprintf ( stderr, "Usage: %s [dir]\n", argv[0]);
		return -1;
	}

	int fd = 0;
	int entry = 0;
	char buf[buf_size];

	if ( ( fd = inotify_init() ) < 0 )
    {
        perror( "Failed to inotify" );
        return -1; 
    }

	if ( ( entry = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE) ) < 0 )
    {
        perror ( "Failed to inotify" );
        close (fd);
    }

	while (1) 
    {

		ssize_t len = read(fd, buf, buf_size);
		if (len < 0) 
        {
            perror ( "Failed to read" );
			close(fd);
			close(entry);
			return -1;
		}

        ssize_t i = 0;
		while (i < len) 
        {
			struct inotify_event *event = (struct inotify_event *) &buf[i];
			if ( event->len ) 
            {
				if (event->mask & IN_CREATE)
                {
					if (event->mask & IN_ISDIR) 
						printf("Directory %s was created\n", event->name);
                    else 
						printf("File %s was created\n", event->name);
				} 
                else if (event->mask & IN_DELETE) 
                {
					if (event->mask & IN_ISDIR) 
						printf("Directory %s was deleted\n", event->name);	
                    else
						printf("The file %s was deleted\n", event->name);
				} 
                else if (event->mask & IN_MODIFY) 
                {
					if (event->mask & IN_ISDIR) 
						printf("Directory %s was modified\n", event->name);
				    else 
						printf("File %s was modified\n", event->name);
				}
			}
			i += buf_size + event->len;
		}
	}
	if (inotify_rm_watch (fd, entry) < 0)  //удаляем наблюдение
    {
		perror ("Failed to inotify_rm_watch");
        close ( fd );
        close ( entry );
        return -1;
	}
	close (fd);
    close (entry);

	return 0;
}