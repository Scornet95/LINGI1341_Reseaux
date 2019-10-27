#ifndef __LINKEDLIST_H_
#define __LINKEDLIST_H_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "packet_interface.h"
#include "utils.h"
#include "structs.h"

address_node * create_address_node(address_t * address);

struct linked_list * create_linked_list();

void enqueue_linked_list(struct linked_list *q, address_t *address);

void delete_address_node(address_node *node);

void destroy_linked_list(struct linked_list *q);

address_t * search_linked_list(struct linked_list* q, struct sockaddr_in6* socket_address, int maxSize, char* format, int* count, uint16_t length);

void remove_linked_list(struct linked_list* q, address_t* address);

ackQueue* createAckQueue();

void enqueue_ack_queue(pkt_t* ack, struct sockaddr_in6* address, ackQueue* queue);

ackNode* dequeue_ack_queue(ackQueue* queue);

void deleteAckQueue(ackQueue* queue);

address_t* createAddress_t(struct sockaddr_in6* add, int count, char* format);
#endif
