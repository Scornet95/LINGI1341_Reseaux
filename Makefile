CC=gcc

CFLAGS= -Wall -g -Wextra -I src/hFiles
LDFLAGS = -lz

OBJ= packet_interface.o utils.o ordered_ll.o linked_list.o
SRC= src/packet_interface.c src/ordered_ll.c src/utils.c src/linked_list.c
TESTSRC= tests/testOrderedLL.c




make: $(OBJ) receiver

receiver : src/receiver.c $(OBJ)
	$(CC) $(CFLAGS) src/receiver.c -o receiver $(OBJ) $(LDFLAGS)

receiver.o : src/receiver.c
	$(CC) $(CFLAGS) -c src/receiver.c $(OBJ) $(LDFLAGS)

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

all: make  clean

tests: $(TESTSRC) $(OBJ) receiver.o
	$(CC) $(CFLAGS) $(TESTSRC) $(OBJ) receiver.o $(LDFLAGS) -o test

clean:
	rm *.o
