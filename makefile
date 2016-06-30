CC=gcc
DEPS = %.h
CFLAGS = 

all: owiarmsrly

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
owiarmsrly: main.o owiarm.o
	$(CC) -o owiarmsrly main.o owiarm.o -lusb-1.0
	
clean:
	rm -f owiarmsrly *.o
	