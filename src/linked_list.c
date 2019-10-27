#include "linked_list.h"

address_node * create_address_node(address_t* address){
    address_node *new_node = malloc(sizeof(address_node));
    if(new_node == NULL){
        fprintf(stderr, "fail to create a node\n");
    }
    new_node->sender_address = address;
    new_node->next = NULL;
    return new_node;
}

struct linked_list * create_linked_list(){
    struct linked_list *q = malloc(sizeof(struct linked_list));
    q->front = NULL;
    q->size = 0;
    return q;
}

void enqueue_linked_list(struct linked_list *q, address_t * address){
    if(address == NULL || q == NULL){
        printf("erreur\n");
        return;
    }
    if(q->size == 0){
        address_node* n = create_address_node(address);
        q->front = n;
        q->size = 1;
    }
    else if(q->front != NULL){
        address_node* n = q->front;
        while(n->next != NULL)
            n = n->next;
        n->next = create_address_node(address);
        q->size = q->size + 1;
    }
    else{
        printf("size of the q is not 0 but the head of the q is NULL\n");
    }
}

void delete_address_node(address_node *n){
    free(n->sender_address->address);
    free(n->sender_address->buffer);
    free(n->sender_address->acks);
    free(n->sender_address);
    free(n);
}


void destroy_linked_list(struct linked_list *q){
    if (q->size == 0){
        free(q);
        q = NULL;
        return;
    }
    else{
        address_node * current = q->front;
        address_node * next;
        while(current != NULL){
            next = current->next;
            delete_address_node(current);
            current = next;
        }
    }
    free(q);
    q = NULL;
    return;
}

address_t * search_linked_list(struct linked_list* q, sockaddr_in6* socket_address, int maxSize, char* format, int* count, uint16_t length){
    if (q->size == 0){
        if(length == 0)
            return NULL;
        address_t* toEnq = createAddress_t(socket_address, *count, format);
        *count = *count + 1;
        enqueue_linked_list(q, toEnq);
        return toEnq;
    }
    else{
        address_node * current = q->front;
        while(current != NULL){
            if (memcmp(current->sender_address->address, socket_address, sizeof(sockaddr_in6)) == 0){
                return current->sender_address;
            }
            else{
                current = current->next;
            }
        }
        if(q->size < maxSize){
            if(length == 0)
                return NULL;
            address_t* toEnq = createAddress_t(socket_address, *count, format);
            *count = *count + 1;
            enqueue_linked_list(q, toEnq);
            return toEnq;
        }
        else{
            return NULL;
        }
    }
}

void remove_linked_list(struct linked_list* q, address_t* address){
    if(q->size == 0){
        return;
    }
    address_node* runner = q->front;
    if(memcmp(q->front->sender_address, address, sizeof(address_t)) == 0){
        q->front = runner->next;
        q->size--;
        delete_address_node(runner);
    }
    address_node* nextRunner = runner->next;
    while(nextRunner != NULL){
        if(memcmp(nextRunner->sender_address, address, sizeof(address_t)) == 0){
            runner->next = nextRunner->next;
            q->size--;
            delete_address_node(nextRunner);
            return;
        }
        runner = runner->next;
        nextRunner = nextRunner->next;
    }
}

address_t* createAddress_t(struct sockaddr_in6* add, int count, char* format){
    address_t* toRet = malloc(sizeof(address_t));
    toRet->address = malloc(sizeof(struct sockaddr_in6));
    memcpy(toRet->adress, add, sizeof(struct sockaddr_in6));
    toRet->last_ack = 0;
    toRet->window = 31;
    toRet->buffer = create_ordered_ll();
    toRet->acks = create_ordered_ll();
    char* s = malloc(strlen(format) + 1);
    sprintf(s, format, count);
    toRet->fd = open(s, O_CREAT | O_APPEND | O_WRONLY | O_TRUNC);
    return toRet;
}







ackQueue* createAckQueue(){
    ackQueue* queue = malloc(sizeof(ackQueue));
    queue->size = 0;
    queue->front = NULL;
    return queue;
}

void enqueue_ack_queue(pkt_t* ack, struct sockaddr_in6* address, ackQueue* queue){
    ackNode* node = malloc(sizeof(ackNode));
    node->address = address;
    node->ack = ack;
    node->next = NULL;

    if(queue->size > 0){
        ackNode* runner = queue->front;

        while(runner->next != NULL){
            runner = runner->next;
        }
        runner->next = node;
        queue->size++;
    }
    else{
        queue->front = node;
        queue->size = 1;
    }
}

ackNode* dequeue_ack_queue(ackQueue* queue){
    if(queue->size == 0)
        return NULL;
    ackNode* first = queue->front;
    queue->front = first->next;
    queue->size--;
    return first;
}

void deleteAckQueue(ackQueue* queue){
    if(queue->size == 0){
        free(queue);
    }
    else{
        ackNode* runner;
        while(queue->size > 0){
            runner = dequeue_ack_queue(queue);
            free(runner->address);
            pkt_del(runner->ack);
        }
        free(queue);
    }
}
