CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = main.o 

%.o: %.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

crawler: $(OBJ)
		gcc $(CFLAGS) -o $@ $^

clean:
		rm -f crawler $(OBJ)