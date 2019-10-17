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
  int err;
  if (sfd < 0){
    perror("Pas de numero valide");
  }
  char buf_r[1024];
  char buf_w[1024];
  fd_set rdfs;
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 4;
  while(1){
    memset((void *) buf_r,0,1024);
    memset((void *) buf_w,0,1024);
    int ret = 0;
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    FD_SET(sfd, &rdfs);
    ret = select(sfd+1,&rdfs,NULL,NULL,&timeout);
    if (ret < 0)
      perror("select()");
    }
    if (FD_ISSET(0,&rdfs)){
        err = read(STDIN_FILENO,&buf_r,1024);
        if (err == -1){
          perror("Impossible de lire sur la sortie standard");
        }
        else{
          int err_w = write(sfd,buf_r,err);
          if (err_w < 0){
            perror("Impossible d'écrire dans le file descriptor");
          }
        }
      }
    if (FD_ISSET(sfd,&rdfs)){
        err = read(sfd,&buf_w,1024);
        if (err < 0){
          perror("Impossible de lire sur le file descriptor");
        }
        else{
          int err_w_2 = write(STDOUT_FILENO,buf_w,err);
          if (err_w_2 < 0){
            perror("Impossible d'écrire sur la sortie standard");
        }
      }
    }
  }
