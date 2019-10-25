CC=gcc

CFLAGS= -Wall -g -Wextra -I src/hFiles 
LDFLAGS = -lz

OBJ= packet_interface.o utils.o ordered_ll.o
SRC= src/packet_interface.c src/ordered_ll.c src/utils.c
TESTSRC= tests/testOrderedLL.c




make: $(OBJ) receiver

receiver : src/receiver.c $(OBJ)
	$(CC) $(CFLAGS) src/receiver.c -o receiver $(OBJ) $(LDFLAGS)

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

all: make  clean

tests: make	 testFolder

testFolder: $(TESTSRC) $(OBJ)
	$(CC) $(CFLAGS) $(TESTSRC) $(OBJ) $(LDFLAGS) -o test
	./test

clean:
	rm *.o
