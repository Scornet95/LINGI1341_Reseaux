#include "ordered_ll.h"
#include "utils.h"
#include "receiver.h"

void testCreateDeleteNode(pkt_t* pkt){
    node* node = create_node(pkt);

    if(node == NULL)
        printf("failed to create a node, malloc failed\n");

    if(pkt != node->pkt)
        printf("the pkt was not properly set inside the node\n");

    if(node->next != NULL)
        printf("node->next should be NULL\n");

    delete_node(node);

    if(node != NULL){
        printf("node should point to NULL after deleting it.\n");
        if(node->pkt != NULL)
            printf("even the packet inside the node isn't free'd, what are you doing\n");
    }
}

void test_create_delete_ordered_ll(){
    ordered_ll* q = create_ordered_ll();

    if(q == NULL)
        printf("failed to create the Linked List\n");

    if(q->front != NULL)
        printf("the fist node should be null when creating the list\n");

    if(q->size != 0)
        printf("the size of the list should be 0 when creating it\n");

    destroy_ll(q);

    if(q != NULL){
        printf("list should point to NULL after destroying it\n");
        if(q->size != 0)
            printf("this is really disappointing, even the size isn't set to 0 when the list is destroyed\n");
        if(q->front != NULL)
            printf("there are still some nodes remaining inside the list\n");
    }
}

void testAckEncode(){
    printf("testing the encoding of an ACK\n\n");

    uint8_t seqnum = (uint8_t) 123;
    uint32_t timestamp = (uint32_t) 12345;
    uint8_t window = (uint8_t) 30;

    pkt_t* pkt = ackEncode(seqnum, timestamp, 1, window);

    printPkt(pkt);

    if(pkt_get_type(pkt) != 2)
        printf("an ack should always hava a type of 2\n");

    if(pkt_get_tr(pkt) != 0)
        printf("an ack cannot be truncated !\n");

    if(pkt_get_window(pkt) != window)
        printf("window was not set properly\n");

    if(pkt_get_length(pkt) != 0)
        printf("an ack doesn't have a payload therefore its size should be set to 0\n");

    if(pkt_get_seqnum(pkt) != seqnum)
        printf("seqnum was not set properly\n");

    if(pkt_get_timestamp(pkt) != timestamp)
        printf("timestamp was not set properly\n");

    pkt_del(pkt);

    printf("testing the encoding of a NACK\n\n");

    pkt = ackEncode(seqnum, timestamp, 0, window);

    printPkt(pkt);

    if(pkt_get_type(pkt) != 3)
        printf("a NACK should always hava a type of 3\n");

    if(pkt_get_tr(pkt) != 0)
        printf("a NACK cannot be truncated !\n");

    if(pkt_get_window(pkt) != window)
        printf("window was not set properly\n");

    if(pkt_get_length(pkt) != 0)
        printf("a NACK doesn't have a payload therefore its size should be set to 0\n");

    if(pkt_get_seqnum(pkt) != seqnum)
        printf("seqnum was not set properly\n");

    if(pkt_get_timestamp(pkt) != timestamp)
        printf("timestamp was not set properly\n");

    pkt_del(pkt);
}

void testAdd(){
    pkt_t * pkt_1 = pkt_new();
    pkt_set_tr(pkt_1, 0);
    pkt_set_type(pkt_1, 1);
    pkt_set_window(pkt_1, 31);
    pkt_set_length(pkt_1, 4);
    pkt_set_timestamp(pkt_1, 127);
    pkt_set_seqnum(pkt_1, 1);
    char buf[4];
    buf[0] = 'A';
    buf[1] = 'B';
    buf[2] = 'C';
    buf[3] = 'D';
    pkt_set_payload(pkt_1,buf, 4);

    pkt_t * pkt_2 = pkt_new();
    pkt_set_tr(pkt_2, 0);
    pkt_set_type(pkt_2, 1);
    pkt_set_window(pkt_2, 31);
    pkt_set_length(pkt_2, 130);
    pkt_set_timestamp(pkt_2, 127);
    pkt_set_seqnum(pkt_2, 255);
    char *buf_2 = malloc(sizeof(char)*130);
    for (int i = 0; i < 130; i++) {
        if (i == 129){
            buf_2[i] = 'B';
        }
        else{
            buf_2[i] = 'A';
        }
    }
    pkt_set_payload(pkt_2,buf_2,130);

    pkt_t * pkt_3 = pkt_new();
    pkt_set_tr(pkt_3, 0);
    pkt_set_type(pkt_3, 1);
    pkt_set_window(pkt_3, 31);
    pkt_set_length(pkt_3, 4);
    pkt_set_timestamp(pkt_3, 127);
    pkt_set_seqnum(pkt_3, 0);
    char buf_3[4];
    buf_3[0] = 'A';
    buf_3[1] = 'B';
    buf_3[2] = 'C';
    buf_3[3] = 'D';
    pkt_set_payload(pkt_3,buf_3, 4);

    pkt_t * pkt_4 = pkt_new();
    pkt_set_tr(pkt_4, 0);
    pkt_set_type(pkt_4, 1);
    pkt_set_window(pkt_4, 31);
    pkt_set_length(pkt_4, 4);
    pkt_set_timestamp(pkt_4, 127);
    pkt_set_seqnum(pkt_4, 254);
    char buf_4[4];
    buf_4[0] = 'A';
    buf_4[1] = 'B';
    buf_4[2] = 'C';
    buf_4[3] = 'D';
    pkt_set_payload(pkt_4,buf_4, 4);


    ordered_ll * queue = create_ordered_ll();
    if (add(queue,pkt_1,253) != 0){
        printf("problème avec le add");
    }
    if (add(queue,pkt_2,253) != 0){
        printf("problème avec le add pkt_2");
    }
    if (add(queue,pkt_3,253) != 0){
        printf("problème avec le add");
    }
    if (add(queue,pkt_4,253) != 0){
        printf("problème avec le add pkt_2");
    }
    printQ(queue);
}

void testRetrieve(){
    uint8_t seqnum = (uint8_t) 123;
    uint32_t timestamp = (uint32_t) 12345;
    uint8_t window = (uint8_t) 30;

    pkt_t* ack = ackEncode(seqnum, timestamp, 1, window);
    pkt_t* nack = ackEncode(seqnum, timestamp, 0, window);

    ordered_ll* q = create_ordered_ll();

    for(int i = 0; i < 10 ; i++){
        if(i % 2 == 0)
            enqueue(q, ack);
        else
            enqueue(q, nack);
    }
    pkt_t* retrieved;

    while(q->size > 0){
        retrieved = retrieve(q);
        printPkt(retrieved);
    }
    destroy_ll(q);
}

void testEmptyBuffer1(){
    address_t adda;
    adda.buffer = create_ordered_ll();
    adda.acks = create_ordered_ll();
    adda.last_ack = 1;
    adda.window = 31;
    adda.fd = open("zeb", O_CREAT | O_APPEND | O_WRONLY | O_TRUNC);

    pkt_t * pkt_1 = pkt_new();
    pkt_set_tr(pkt_1, 0);
    pkt_set_type(pkt_1, 1);
    pkt_set_window(pkt_1, 31);
    pkt_set_length(pkt_1, 4);
    pkt_set_timestamp(pkt_1, 127);
    pkt_set_seqnum(pkt_1, 4);
    char buf[4];
    buf[0] = 'A';
    buf[1] = 'B';
    buf[2] = 'C';
    buf[3] = 'D';
    pkt_set_payload(pkt_1,buf, 4);

    pkt_t * pkt_2 = pkt_new();
    pkt_set_tr(pkt_2, 0);
    pkt_set_type(pkt_2, 1);
    pkt_set_window(pkt_2, 31);
    pkt_set_length(pkt_2, 130);
    pkt_set_timestamp(pkt_2, 127);
    pkt_set_seqnum(pkt_2, 3);
    char *buf_2 = malloc(sizeof(char)*130);
    for (int i = 0; i < 130; i++) {
        if (i == 129){
            buf_2[i] = 'B';
        }
        else{
            buf_2[i] = 'A';
        }
    }
    pkt_set_payload(pkt_2,buf_2,130);

    pkt_t * pkt_3 = pkt_new();
    pkt_set_tr(pkt_3, 0);
    pkt_set_type(pkt_3, 1);
    pkt_set_window(pkt_3, 31);
    pkt_set_length(pkt_3, 4);
    pkt_set_timestamp(pkt_3, 127);
    pkt_set_seqnum(pkt_3, 1);
    char buf_3[4];
    buf_3[0] = 'S';
    buf_3[1] = 'T';
    buf_3[2] = 'O';
    buf_3[3] = 'M';
    pkt_set_payload(pkt_3,buf_3, 4);

    pkt_t * pkt_4 = pkt_new();
    pkt_set_tr(pkt_4, 0);
    pkt_set_type(pkt_4, 1);
    pkt_set_window(pkt_4, 31);
    pkt_set_length(pkt_4, 4);
    pkt_set_timestamp(pkt_4, 127);
    pkt_set_seqnum(pkt_4, 2);
    char buf_4[4];
    buf_4[0] = 'Q';
    buf_4[1] = 'V';
    buf_4[2] = 'W';
    buf_4[3] = 'Z';
    pkt_set_payload(pkt_4,buf_4, 4);

    add(adda.buffer, pkt_1, 255);
    add(adda.buffer, pkt_2, 255);
    add(adda.buffer, pkt_3, 255);
    add(adda.buffer, pkt_4, 255);

    printQ(adda.buffer);

    emptyBuffer(&adda);
    printf("zeb\n");

    printQ(adda.buffer);

    printf("lastAck : %u\n", adda.last_ack);

    printf("\n\n");
    printQ(adda.acks);

    destroy_ll(adda.acks);
    destroy_ll(adda.buffer);

}

void testPktVerif(){
    pkt_t * pkt_1 = pkt_new();
    pkt_set_tr(pkt_1, 0);
    pkt_set_type(pkt_1, 1);
    pkt_set_window(pkt_1, 31);
    pkt_set_length(pkt_1, 4);
    pkt_set_timestamp(pkt_1, 127);
    pkt_set_seqnum(pkt_1, 0);
    char buf[4];
    buf[0] = 'A';
    buf[1] = 'B';
    buf[2] = 'C';
    buf[3] = 'D';
    pkt_set_payload(pkt_1,buf, 4);

    pkt_t * pkt_2 = pkt_new();
    pkt_set_tr(pkt_2, 1);
    pkt_set_type(pkt_2, 1);
    pkt_set_window(pkt_2, 31);
    pkt_set_length(pkt_2, 130);
    pkt_set_timestamp(pkt_2, 127);
    pkt_set_seqnum(pkt_2, 3);
    char *buf_2 = malloc(sizeof(char)*130);
    for (int i = 0; i < 130; i++) {
        if (i == 129){
            buf_2[i] = 'B';
        }
        else{
            buf_2[i] = 'A';
        }
    }
    pkt_set_payload(pkt_2,buf_2,130);

    pkt_t * pkt_3 = pkt_new();
    pkt_set_tr(pkt_3, 0);
    pkt_set_type(pkt_3, 1);
    pkt_set_window(pkt_3, 31);
    pkt_set_length(pkt_3, 4);
    pkt_set_timestamp(pkt_3, 127);
    pkt_set_seqnum(pkt_3, 245);
    char buf_3[4];
    buf_3[0] = 'S';
    buf_3[1] = 'T';
    buf_3[2] = 'O';
    buf_3[3] = 'M';
    pkt_set_payload(pkt_3,buf_3, 4);

    pkt_t * pkt_4 = pkt_new();
    pkt_set_tr(pkt_4, 0);
    pkt_set_type(pkt_4, 1);
    pkt_set_window(pkt_4, 31);
    pkt_set_length(pkt_4, 4);
    pkt_set_timestamp(pkt_4, 127);
    pkt_set_seqnum(pkt_4, 2);
    char buf_4[4];
    buf_4[0] = 'Q';
    buf_4[1] = 'V';
    buf_4[2] = 'W';
    buf_4[3] = 'Z';
    pkt_set_payload(pkt_4,buf_4, 4);

    pkt_t * pkt_5 = pkt_new();
    pkt_set_tr(pkt_5, 0);
    pkt_set_type(pkt_5, 1);
    pkt_set_window(pkt_5, 31);
    pkt_set_length(pkt_5, 4);
    pkt_set_timestamp(pkt_5, 127);
    pkt_set_seqnum(pkt_5, 2);
    char buf_5[4];
    buf_5[0] = 'Q';
    buf_5[1] = 'V';
    buf_5[2] = 'W';
    buf_5[3] = 'Z';
    pkt_set_payload(pkt_5,buf_5, 4);

    if ((pkt_verif(pkt_1,254,31)) == 3){
        printf("Succeed pkt_1\n");
    }
    else{
        printf("failed to verif pkt_1\n");
    }
    int q = pkt_verif(pkt_2,254,31);
    printf("Succeed pkt_2 %d\n",q);
    
    if ((pkt_verif(pkt_3,254,31)) == 2){
        printf("Succeed Pkt_3\n");
    }
    else{
        printf("failed to verif pkt_3\n");
    }
    if ((pkt_verif(pkt_4,4,31)) == 2){
        printf("Succeed Pkt_4\n");
    }
    else{
        printf("failed to verif pkt_4\n");
    }
    if ((pkt_verif(pkt_5,1,31)) == 3){
        printf("Succeed Pkt_5\n");
    }
    else{
        printf("failed to verif pkt_5\n");
    }


}

int main(){
    testPktVerif();
    /*
    printf("Starting the test suite on ordered_ll\n\n");
    printf("testing ackEncode\n\n");
    testAdd();
    */
    return EXIT_SUCCESS;
}
