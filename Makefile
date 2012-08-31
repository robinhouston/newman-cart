CFLAGS = -O7
#CFLAGS = -g
#CFLAGS = -p
CC = gcc
#LIBOPTS = -static
LIBOPTS =
LIBS = -lfftw3 -lm

cart:main.o cart.o
	$(CC) $(CFLAGS) $(LIBOPTS) -o cart main.o cart.o $(LIBS)

cart.o:cart.c Makefile
	$(CC) $(CFLAGS) -c cart.c

main.o:main.c Makefile
	$(CC) $(CFLAGS) -c main.c

interp:interp.o
	$(CC) $(CFLAGS) -o interp interp.o

interp.o:interp.c Makefile
	$(CC) $(CFLAGS) -c interp.c

clean:
	rm -fv *.o

all: cart interp
