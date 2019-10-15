##include "create_socket.h"
#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */

/* Creates a socket and initialize it
 * @source_addr: if !NULL, the source address that should be bound to this socket
 * @src_port: if >0, the port on which the socket is listening
 * @dest_addr: if !NULL, the destination address to which the socket should send data
 * @dst_port: if >0, the destination port to which the socket should be connected
 * @return: a file descriptor number representing the socket,
 *         or -1 in case of error (explanation will be printed on stderr)
 */
int create_socket(struct sockaddr_in6 *source_addr,int src_port,struct sockaddr_in6 *dest_addr,int dst_port){
  int sockfd;
  if ((sockfd = socket(PF_INET6, SOCK_STREAM, 0)) < 0 ){
    return -1;
  }
  if (source_addr->sin6_addr != NULL){
    int err = bind(sockfd, (struct sockaddr *) &source_addr,sizeof(source_addr));
    if (err < 0){
      return -1;
    }

  }
  if (src_port >= 0){
    if (setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, sizeof(int), optlen)); // Pour connecter ke port, a finir
  }
  if ((connect(sockfd, dest_addr, sizeof(dest_addr))) < 0){
    return -1;
  }
  return sockfd;

}
