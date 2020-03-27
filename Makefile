CC = gcc
CFLAGS = -Wall
DEPS = getip.h
OBJ = main.o getip.o

%.o: %.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

crawler: $(OBJ)
		gcc $(CFLAGS) -o $@ $^

clean:
		rm -f crawler $(OBJ)