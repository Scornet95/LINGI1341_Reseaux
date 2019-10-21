#include "ordered_ll.h"

node * create_node(){
    node *new_node = malloc(sizeof(node));
    if(new_node == NULL){
        fprintf(stderr, "fail to create a node\n");
    }
    new_node->pkt = pkt_new();
    new_node->next = NULL;
    return new_node;
}

ordered_ll * create_ordered_ll(){
    ordered_ll *q = malloc(sizeof(ordered_ll));
    q->front = NULL;
    q->size = 0;
    return q;
}

int enqueue(ordered_ll *q, pkt_t *pkt){
    if(q==NULL){return -1;}
    if(pkt==NULL){return -2;}
    node *new_node = create_node();
    new_node->pkt = pkt;
    new_node->next = q->front;
    q->front = new_node;
    free(new_node);
    return 0;
}
int add(ordered_ll * q, pkt_t *pkt){
    if(q==NULL){return -1;}
    if(pkt==NULL){return -2;}
    int check_seqnum;
    int actual_seqnum;
    node * new_node = create_node();
    new_node->pkt = pkt;
    if (q->front == NULL){
        q->front = new_node;
        q->size++;
        return 0;
    }
    node * current = q->front;
    actual_seqnum = pkt_get_seqnum(pkt);
    if (current->pkt->seqnum > actual_seqnum){
        new_node->next = current;
        q->front = new_node;
        q->size++;
        return 0;
    }
    while(current->next != NULL){
        check_seqnum = pkt_get_seqnum(current->next->pkt);
        if (check_seqnum > actual_seqnum){
            new_node->next = current->next;
            current->next = new_node;
            q->size++;
            return 0;
        }
        current = current->next;
    }
    current->next = new_node;
    q->size++;
    return 0;
}

pkt_t * retrieve(ordered_ll * q){
    if (q->size == 0){
        fprintf(stderr, "There is nothing in the queue\n");
    }
    pkt_t * new_pkt;
    new_pkt = q->front->pkt;
    node *n = create_node();
    n = q->front;
    if (q->size == 1){
        free(n);
        q->front = NULL;
        q->size = q->size-1;
        return new_pkt;
    }
    q->front = n->next;
    q->size = q->size - 1;
    free(n);
    return new_pkt;
}

void destroy_ll(ordered_ll *q){
    if (q == NULL){
        free(q);
        return;
    }
    else{
        while(q->size != 0){
            free(retrieve(q));
        }
    }
    free(q);
    return;
}


int printQ(ordered_ll *q){
    while(q->size!=0){
        //pkt_t* pek=pkt_new();
        printf("%d\n",q->size);
        retrieve(q);
        //printf("%d\n",(int)pkt_get_type(pek));
    }
    return 0;
}
