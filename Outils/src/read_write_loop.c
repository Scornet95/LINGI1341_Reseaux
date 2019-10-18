#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "read_write_loop.h"


/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(const int sfd){
    if (sfd < 0){
      perror("Pas de numero valide");
    }
    char writeBuf[1024]; //Le buffer dans lequel on va récupérer ce qui vient de stdin
    char readBuf[1024]; //Le buffer dans lequel on va récupérer ce qui vient du socket.

    struct pollfd pfds[2];

    while(1){
        int size;
        pfds[0].fd = STDIN_FILENO;
        pfds[0].events = POLLIN;

        pfds[1].fd = sfd;
        pfds[1].events = POLLIN | POLLOUT;

        poll(pfds, 2, -1);

        if(pfds[0].revents & POLLIN){
            size = read(STDIN_FILENO, writeBuf, 1024);
            if(size == 0)
                return;
            if(pfds[1].revents & POLLOUT){
                write(sfd, writeBuf, size);
            }
        }

        if(pfds[1].revents & POLLIN){
            size = read(sfd, readBuf, 1024);
            if(size != 0)
                write(STDOUT_FILENO, readBuf, size);
        }
    }
}
