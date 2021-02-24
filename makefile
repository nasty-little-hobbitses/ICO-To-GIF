CC=gcc
CFLAGS=-I.

all_sources = Proj.c
all_objs = $(all_sources: .c=.o)
%.o: %.c
	$(CC) -c $(all_sources) $(CFLAGS)
ICO-To-GIF: $(all_objs)
	$(CC) -o ICO-To-GIF $(all_sources)
clean:
	rm -rf ICO-To-GIF *.o 
