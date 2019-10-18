#include "create_socket.h"
#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */
#include <stdlib.h>
#include <stdio.h>

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
    sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0){
        return -1;
    }
    if(source_addr != NULL && src_port > 0){
        source_addr->sin6_port = htons(src_port);
        source_addr->sin6_family = AF_INET6;
        int err = bind(sockfd,(struct sockaddr*) source_addr, sizeof(struct sockaddr_in6));
        if (err < 0){
            fprintf(stderr, "error while binding the socket to source adress\n");
            return -1;
        }
    }
    else if(dest_addr != NULL && dst_port > 0){
        dest_addr->sin6_port = htons(dst_port);
        dest_addr->sin6_family = AF_INET6;
        int err = connect(sockfd,(struct sockaddr*) dest_addr, sizeof(struct sockaddr_in6));
        if (err < 0){
            fprintf(stderr, "error while connecting the socket to dest\n");
            return -1;
        }
    }
    else{
        fprintf(stderr, "No valid source or dest addr/port\n");
        return -1;
    }
    return sockfd;
}
