#ifndef __ORDEREDLL_H_
#define __ORDEREDLL_H_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "packet_interface.h"

typedef struct node{
    pkt_t * pkt;
    struct node *next;
    int index;
}node;

typedef struct queue{
    node *front;
    int size;
}ordered_ll;


void enqueue(ordered_ll *q, pkt_t *pkt);

void destroy_ll(ordered_ll *q);

node * create_node(pkt_t* pkt);

void delete_node(node *node);

int printQ(ordered_ll *q);

ordered_ll * create_ordered_ll();

int add(ordered_ll * q, pkt_t *pkt, int lastAck);

pkt_t * retrieve(ordered_ll * linked_list);

void delete_node(node *node);

uint8_t peek(ordered_ll * list);

#endif
