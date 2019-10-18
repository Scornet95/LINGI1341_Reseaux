CC=gcc

CFLAGS= -Wall -g -Wextra -I hFiles
CFLAGS += -Wshadow
LDFLAGS = -lz

OBJ=create_socket.o read_write_loop.o real_address.o wait_for_client.o
SRC= src/create_socket.c src/read_write_loop.c src/real_address.c src/wait_for_client.c




make: $(OBJ) chat

chat : src/chat.c $(OBJ)
	$(CC) $(CFLAGS) src/chat.c -o chat $(OBJ) $(LDFLAGS)

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

all: make  clean

clean:
	rm *.o 
