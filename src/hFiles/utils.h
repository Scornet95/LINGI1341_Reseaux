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

struct param_t{
    struct sockaddr_in6 *adress;
    int port;
    char* format;
    int maxCo;
};

struct adress_t{
    struct sockaddr_in6 *adress;
    int last_ack;
    pkt_t *buffer[];

};

int pkt_verif(pkt_t *pkt,int last_ack);
const char * real_address(const char *address, struct sockaddr_in6 *rval);

struct param_t getArguments(int argc, char* argv[]);

#endif
