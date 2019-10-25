#include "ordered_ll.h"

node * create_node(pkt_t* pkt){
    node *new_node = malloc(sizeof(node));
    if(new_node == NULL){
        fprintf(stderr, "fail to create a node\n");
    }
    new_node->pkt = pkt;
    new_node->next = NULL;
    return new_node;
}

ordered_ll * create_ordered_ll(){
    ordered_ll *q = malloc(sizeof(ordered_ll));
    q->front = NULL;
    q->size = 0;
    return q;
}

void enqueue(ordered_ll *q, pkt_t *pkt){
    if(pkt == NULL || q == NULL){
        printf("erreur\n");
        return;
    }
    if(q->size == 0){
        node* n = create_node(pkt);
        q->front = n;
        q->size = 1;
    }
    else if(q->front != NULL){
        node* n = q->front;
        while(n->next != NULL)
            n = n->next;
        n->next = create_node(pkt);
        q->size = q->size + 1;
    }
    else{
        printf("size of the q is not 0 but the head of the q is NULL\n");
    }
}

void delete_node(node *n){
    free(n->pkt);
    free(n);
}
int add(ordered_ll * q, pkt_t *pkt, int lastAck){
    if(q==NULL){return -1;}
    if(pkt==NULL){return -2;}
    int check_seqnum;
    int actual_seqnum;
    node * new_node = create_node(NULL);
    actual_seqnum = pkt_get_seqnum(pkt);
    if (lastAck > actual_seqnum){
        new_node->index = actual_seqnum + 256;
    }
    else{
        new_node->index = actual_seqnum;
    }
    new_node->pkt = pkt;
    if (q->front == NULL){
        q->front = new_node;
        q->size++;
        return 0;
    }
    node * current = q->front;
    if (current->pkt->seqnum == new_node->index){
        return -1;
    }
    if (current->pkt->seqnum > new_node->index){
        new_node->next = current;
        q->front = new_node;
        q->size++;
        return 0;
    }
    while(current->next != NULL){
        check_seqnum = current->next->index;
        if (check_seqnum == new_node->index){
            return -1;
        }
        if (check_seqnum > new_node->index){
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
        return NULL;
    }
    node* n = q->front;
    if (q->size == 1){
        q->size = 0;
        pkt_t* toRet = n->pkt;
        free(q->front);
        return toRet;
    }
    else{
        q->front = n->next;
        q->size = q->size - 1;
        pkt_t* toRet = n->pkt;
        free(n);
        return toRet;
    }
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
    if(q == NULL){
        printf("The list is NULL, nothing to print\n");
        return 1;
    }
    if(q->front != NULL){
        node* runner = q->front;

        while(runner != NULL){
            printPkt(runner->pkt);
            runner = runner->next;
        }
    }
    else
        printf("The first node is NULL, nothing to print\n");
}
uint8_t peek(ordered_ll *list){
    if(list->front == NULL){
        return -1;
    }
    return pkt_get_seqnum((list->front)->pkt);
}
