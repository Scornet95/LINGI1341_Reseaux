#include "ordered_ll.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

node * create_node(pkt_t *pkt){
    node *new_node;
    new_node = malloc(sizeof(node));
    new_node->pkt = malloc(sizeof(pkt))
    memcpy(new_node->pkt,pkt,sizeof(pkt));
    new_node->next = NULL;
    return new_node;
}

ordered_ll * create_ordered_ll(){
    ordered_ll *q;
    q = malloc(sizeof(ordered_ll));
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

void add(ordered_ll * q, pkt_t *pkt){
    node * new_node = create_node(pkt);
    if (q->rear == NULL){
        q->rear = q->front = new_node;
        q->size++;
        return;
    }
    if (q->front->seqnum > pkt->seqnum){
        new_node->next = q->front;
        q->front = new_node;
        q->size++;
        return;
    }
    if (q->rear->seqnum < pkt->seqnum){
        q->rear->next = new_node;
        q->rear = new_node;
        q->size++;
        return;
    }
    node * curseur = q->front;
    while(curseur->next->pkt->seqnum < pkt->seqnum){
        curseur = curseur->next;
    }
    new_node->next = curseur->next;
    curseur->next = new_node;
    q->size++;
    free(curseur);
    return;
}

pkt_t * retrieve(ordered_ll * linked_list){
    if (linked_list->size == 0){
        fprintf(stderr, "There is nothing in the queue\n");
    }
    pkt_t * new_pkt = pkt_new();
    memcpy(new_pkt,linked_list->front->pkt,sizeof(linked_list->front->pkt));
    node * free_node = linked_list->front;
    linked_list->front = linked_list->front->next;
    if(linked_list->front == NULL){
        linked_list->rear = NULL;
    }
    free(free_node);
    linked_list->size--;
    return new_pkt;
}
