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
int main() {
    ordered_ll* q = create_ordered_ll();
    //Node *n=malloc(sizeof(struct Node));

    pkt_t * pkt = pkt_new();
	size_t * len = (size_t*) malloc(sizeof(size_t));
	*len = 30;
	char * buf = (char *) malloc(30);
	buf[0] = (char) 0x5c;
	buf[1] = (char) 0x0b;
	buf[2] = (char) 0x7b;
	buf[3] = (char) 0x17;
	buf[4] = (char) 0x00;
	buf[5] = (char) 0x00;
	buf[6] = (char) 0x00;
	buf[7] = (char) 0x50;
	buf[8] = (char) 0x12;
	buf[9] = (char) 0x12;
	buf[10] = (char) 0x86;
	buf[11] = (char) 0x68;
	buf[12] = (char) 0x65;
	buf[13] = (char) 0x6c;
	buf[14] = (char) 0x6c;
	buf[15] = (char) 0x6f;
	buf[16] = (char) 0x20;
	buf[17] = (char) 0x77;
	buf[18] = (char) 0x6f;
	buf[19] = (char) 0x72;
	buf[20] = (char) 0x6c;
	buf[21] = (char) 0x64;
	buf[22] = (char) 0x0d;
	buf[23] = (char) 0x4a;
	buf[24] = (char) 0x11;
	buf[25] = (char) 0x85;

	pkt_status_code err = pkt_decode(buf, 26, pkt);
	if( err == E_TYPE) printf("type \n");
	if( err == E_LENGTH) printf("length \n");
	if( err == E_CRC) printf("crc \n");
	if( err == E_WINDOW) printf("window \n");
	if( err == E_SEQNUM) printf("seqnum\n");
	if( err == E_NOMEM) printf("nomem\n");
	if( err == E_NOHEADER) printf("header\n");
	if( err == PKT_OK) printf("okok \n");
	if( err == E_UNCONSISTENT) printf("unconsistent \n");

    int test = add(q,pkt);
    if(test!=0){
        printf("err1or");
    }

    int test2=add(q,pkt);
    if(test2!=0){
        printf("error");
    }

    int test3=add(q,pkt);
    if(test3!=0){
        printf("error");
    }

    //printf("enqueued");

    printQ(q);
    printf("la taille apres deque %d\n", q->size);

    /*pkt_t *pK = pkt_new();
    if((int)pkt_set_type(pK, PTYPE_ACK)<0){return -1;}
    enqueue(q, pK);
    printQ(q);*/



    return 0;
}
