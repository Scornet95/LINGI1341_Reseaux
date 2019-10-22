#ifndef __UTILS_H
#define __UTILS_H

#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <stdint.h> /* uintx_t */
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "packet_interface.h"
#include "ordered_ll.h"

typedef struct param_t{
    struct sockaddr_in6 *address;
    int port;
    char* format;
    int maxCo;
}param_t;

typedef struct address_t{
    struct sockaddr_in6 *address;
    int last_ack;
    int fd;
    int window;
    int firstGo;
    uint32_t timestamp;
    ordered_ll *buffer;
    ordered_ll *acks;
}address_t;

int pkt_verif(pkt_t *pkt, int last_ack);

const char * real_address(const char *address, struct sockaddr_in6 *rval);

struct param_t getArguments(int argc, char* argv[]);

int create_socket(struct sockaddr_in6 *source_addr,int src_port,struct sockaddr_in6 *dest_addr,int dst_port);

pkt_t* ackEncode(uint8_t seqnum, uint32_t timestamp, int ack, int window);

int sendQueue(int sockfd, address_t* addie);

int emptyBuffer(address_t* add);

#endif
