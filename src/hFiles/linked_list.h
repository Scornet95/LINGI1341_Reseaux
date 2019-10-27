#ifndef __LINKEDLIST_H_
#define __LINKEDLIST_H_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "packet_interface.h"
#include "utils.h"
#include "structs.h"

address_node * create_address_node(address_t * address);

struct linked_list * create_linked_list();

void enqueue_linked_list(struct linked_list *q, address_node *address);

void delete_address_node(address_node *node);

void destroy_linked_list(struct linked_list *q);

void print_linked_list(struct linked_list *q);

address_t * search_linked_list(struct linked_list * linked_list);

address_node * remove_address_node(struct linked_list * linked_list,address_t * address);

void delete_address_node(address_node *node);

ackQueue* createAckQueue();

int enqueue_ack_queue(pkt_t* ack, struct sockaddr_in6* address, ackQueue* queue);

ackNode* dequeue_ack_queue(ackQueue* queue);

#endif
