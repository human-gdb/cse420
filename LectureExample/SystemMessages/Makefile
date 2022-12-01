CC = gcc # compiler
CFLAGS = -Wall -g # compile flags
LIBS = -lrt # libs

all: sender receiver

sender: sender.o
	$(CC) -o sender sender.o $(LIBS)

receiver: receiver.o
	$(CC) -o receiver receiver.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -f sender receiver *.o *~

