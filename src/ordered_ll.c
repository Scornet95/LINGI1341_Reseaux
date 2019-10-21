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

void enqueue(ordered_ll *q, pkt_t *pkt){
    if(q==NULL){return;}
    if(pkt==NULL){return;}
    int type_pkt = pkt_get_type(pkt);
    if (type_pkt == 3){
        node * new_node = create_node();
        if (q->front == NULL){
            new_node->pkt = pkt;
            new_node->next = NULL;
            q->front = new_node;
            q->size++;
            return;
        }
        else{
            if(q->front->next == NULL){
                new_node->pkt = pkt;
                new_node->next = NULL;
                q->front->next = new_node;
                q->size++;
                return;
            }
            node * curseur_node = q->front;
            while(curseur_node->next != NULL){
                curseur_node = curseur_node->next;
            }
            new_node->pkt = pkt;
            new_node->next = NULL;
            curseur_node->next = new_node;
            q->size++;
            return;
            }
        }
    if (type_pkt == 2){
        if (q->front == NULL){
            node *new_node = create_node();
            printQ(q);
            new_node->pkt = pkt;
            new_node->next = NULL;
            q->front = new_node;
            q->size++;
            return;
        }
        int type = pkt_get_type(q->front->pkt);
        if (type == 3){
            node *new_node = create_node();
            new_node->pkt = pkt;
            new_node->next = q->front;
            q->front = new_node;
            q->size++;
            return;
        }
        int new_seqnum = pkt_get_seqnum(pkt);
        int actual_seqnum = pkt_get_seqnum(q->front->pkt);
        if (new_seqnum >= actual_seqnum){
            node *new_node = create_node();
            new_node->pkt = pkt;
            new_node->next = q->front->next;
            q->front = new_node;
            return;
        }
        else{
            return;
        }
    }
    else{
        fprintf(stderr,"The packet should be of type Nack or Ack\n");
    }
}


void delete_node(node *node){
    free(node->pkt);
    free(node);
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
    if (current->pkt->seqnum == actual_seqnum){
        return -1;
    }
    if (current->pkt->seqnum > actual_seqnum){
        new_node->next = current;
        q->front = new_node;
        q->size++;
        return 0;
    }
    while(current->next != NULL){
        check_seqnum = pkt_get_seqnum(current->next->pkt);
        if (check_seqnum == actual_seqnum){
            return -1;
        }
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
    if (q->size == 1){
        node * n = create_node();
        n = q->front;
        q->front = NULL;
        q->size = q->size-1;
        delete_node(n);
        return new_pkt;
    }
    else{
        node * n;
        n = q->front;
        q->front = q->front->next;
        q->size = q->size - 1;
        return new_pkt;
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
    while(q->size!=0){
        //pkt_t* pek=pkt_new();
        printf("%d\n",q->front->pkt->seqnum);
        retrieve(q);
        //printf("%d\n",(int)pkt_get_type(pek));
    }
    return 0;
}
uint8_t peek(ordered_ll *list){
    if(list->front == NULL){
        return -1;
    }
    return pkt_get_seqnum((list->front)->pkt);
}
