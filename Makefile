CFLAGS = -Wall -Wextra

all: task

task: task.c 
	$(CC) $(CFLAGS) task.c -lm -o task

clean:
	rm -rf task

.PHONY: all clean
