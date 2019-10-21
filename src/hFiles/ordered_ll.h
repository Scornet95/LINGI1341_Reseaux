#ifndef __ORDEREDLL_H
#define __ORDEREDLL_H
#include <stdlib.h>
#include <stdio.h>
#include "packet_interface.h"

typedef struct node{
    pkt_t * pkt;
    struct node *next;
}node;

typedef struct queue{
    node *front;
    node *rear;
    int size;
}ordered_ll;

node * create_node(pkt_t *pkt);

ordered_ll * create_ordered_ll();

void add(ordered_ll * q, pkt_t *pkt);

pkt_t * retrieve(ordered_ll * linked_list);

#endif
