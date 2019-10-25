#include "ordered_ll.h"
#include "utils.h"

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

void testEnqueue(){
    
}

int main(){
    printf("Starting the test suite on ordered_ll\n\n");
    printf("testing ackEncode\n\n");
    testAckEncode();
    return EXIT_SUCCESS;
}
