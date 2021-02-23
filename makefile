CC=gcc
CFLAGS=-I.
DEPS = ourheader.h
all_sources = combined.c
all_objs = $(all_sources: .c=.o)
%.o: %.c $(DEPS)
	echo $(all_sources)
	$(CC) -c $(all_sources) $(CFLAGS)
ICO-To-GIF: $(all_objs)
	$(CC) -o ICO-To-GIF $(all_sources) -lm
clean:
	rm -rf ICO-To-GIF *.o 
