#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "wait_for_client.h"

/* Block the caller until a message is received on sfd,
 * and connect the socket to the source addresse of the received message.
 * @sfd: a file descriptor to a bound socket but not yet connected
 * @return: 0 in case of success, -1 otherwise
 * @POST: This call is idempotent, it does not 'consume' the data of the message,
 * and could be repeated several times blocking only at the first call.
 */
int wait_for_client(int sfd){
  struct sockaddr_in6 dest_addr;
  char buf[1024];
  socklen_t length = (socklen_t) sizeof(struct sockaddr_in6);
  ssize_t err_rec;
  err_rec = recvfrom(sfd,buf,1024,0,(struct sockaddr *)&dest_addr,&length);
  if (err_rec < 0){
    return -1;
  }
  if ((connect(sfd,(const struct sockaddr *)&dest_addr,length)) < 0){
    return -1;
  }
  return 0;
}
