CC = gcc

CFLAGS = -c -Wall -Wextra

all: task

main: task.o 
	$(CC) task.c -lm -o task

main.o: task.c
	$(CC) $(CFLAGS) task.c

clean:
	rm -rf *.o task
