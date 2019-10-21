CC=gcc

CFLAGS= -Wall -g -Wextra -I src/hFiles
CFLAGS += -Wshadow
LDFLAGS = -lz

OBJ= packet_interface.o utils.o ordered_ll.o
SRC= src/packet_interface.c src/utils.c src/ordered_ll.c




make: $(OBJ) receiver

receiver : src/receiver.c $(OBJ)
	$(CC) $(CFLAGS) src/receiver.c -o receiver $(OBJ) $(LDFLAGS)

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

all: make  clean

clean:
	rm *.o
