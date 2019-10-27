#ifndef __STRUCTS_H_
#define __STRUCTS_H_

#include <netinet/in.h>

typedef struct queue ordered_ll;

typedef struct node{
    pkt_t * pkt;
    struct node *next;
    int index;
}node;

struct queue{
    node *front;
    int size;
};

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
    ordered_ll *buffer;
    ordered_ll *acks;
}address_t;

struct linked_list{
    address_node *front;
    int size;
};

typedef struct noeud{
    address_t * sender_address;
    struct noeud *next;
}address_node;

typedef struct ackNode{
    struct sockaddr_in6* address;
    pkt_t* ack;
    struct ackNode* next;
}ackNode;

typedef struct ackQueue{
    ackNode* front;
    int size;
}ackQueue;

#endif
