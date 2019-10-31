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
        return;
    }
    printf("Test concernant la création et la suppression de nodes terminé. Si rien n'a été affiché avant ceci, le test est réussi.\n");
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

    if(q->size != 0)
        printf("this is really disappointing, even the size isn't set to 0 when the list is destroyed\n");
    if(q->front != NULL)
        printf("there are still some nodes remaining inside the list\n");

    printf("Test concernant la création et suppression de Ordered linked list terminé. Si rien n'a été affiché avant ceci, le test est réussi\n");
}

void testAckEncode(){

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

    printf("Test concernant l'encodage des ACK terminé. Si rien n'a été affiché avant ceci, le test est réussi.\n");
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
    printf("ajout du premier paquet dans la liste.\n");
    if (add(queue,pkt_1,253) != 0){
        printf("problème avec le add paquet 1\n");
    }
    printf("ajout du second paquet dans la liste\n");
    if (add(queue,pkt_2,253) != 0){
        printf("problème avec le add paquet 2\n");
    }
    printf("ajout du troisième paquet dans la liste.\n");
    if (add(queue,pkt_3,253) != 0){
        printf("problème avec le add paquet 3\n");
    }
    printf("ajout du quatrième paquet dans la liste\n");
    if (add(queue,pkt_4,253) != 0){
        printf("problème avec le add paquet 4\n");
    }
    printf("La liste après les quatre ajouts : \n");
    printQ(queue);
}

void testRetrieve(){
    uint8_t seqnum = (uint8_t) 123;
    uint32_t timestamp = (uint32_t) 12345;
    uint8_t window = (uint8_t) 30;

    printf("Nous allons ajouter 5 ack et 5 nack dans la liste, les ack aux indices pairs et nacks aux indices impairs.\n");

    pkt_t* ack = ackEncode(seqnum, timestamp, 1, window);
    printf("le paquet ack : \n");
    printPkt(ack);
    pkt_t* nack = ackEncode(seqnum, timestamp, 0, window);
    printf("le paquet nack : \n");
    printPkt(nack);

    ordered_ll* q = create_ordered_ll();

    for(int i = 0; i < 10 ; i++){
        if(i % 2 == 0)
            enqueue(q, ack);
        else
            enqueue(q, nack);
    }
    pkt_t* retrieved;

    printf("Nous devrions afficher 10 paquets, 5 acks et 5 nacks\n");
    while(q->size > 0){
        retrieved = retrieve(q);
        printPkt(retrieved);
    }
    destroy_ll(q);
}

void testEmptyBuffer(){
    address_t adda;
    adda.buffer = create_ordered_ll();
    adda.acks = create_ordered_ll();
    adda.last_ack = 1;
    adda.window = 31;
    adda.fd = open("testEmptyBuffer", O_CREAT | O_APPEND | O_WRONLY | O_TRUNC);
    printf("Nous allons ajouter 4 paquets au buffer de réception fictif de notre test et vérifier que leurs payloads sont bien écrits dans le fichier.\n");

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
    printf("paquet 1 : \n");
    printPkt(pkt_1);

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
    printf("paquet 2 : \n");
    printPkt(pkt_2);

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
    printf("paquet 3 : \n");
    printPkt(pkt_3);

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
    printf("paquet 4 : \n");
    printPkt(pkt_4);

    add(adda.buffer, pkt_1, 255);
    add(adda.buffer, pkt_2, 255);
    add(adda.buffer, pkt_3, 255);
    add(adda.buffer, pkt_4, 255);

    printf("buffer après les add : \n");
    printQ(adda.buffer);

    emptyBuffer(&adda, NULL);
    close(adda.fd);

    printf("buffer après l'avoir vidé\n");
    printQ(adda.buffer);

    printf("\nLa queue des acks après avoir vidé le buffer\n");
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

void testPacketImplem(){
    pkt_t * pkt_1 = pkt_new();
    pkt_set_tr(pkt_1,0);
    pkt_set_type(pkt_1,1);
    pkt_set_window(pkt_1,31);
    pkt_set_length(pkt_1,4);
    pkt_set_timestamp(pkt_1,127);
    pkt_set_seqnum(pkt_1,5);
    char buf[4];
    buf[0] = 'A';
    buf[1] = 'B';
    buf[2] = 'C';
    buf[3] = 'D';
    pkt_set_payload(pkt_1,buf,4);
    printf("Paquet 1 avant l'encodage : \n");
    printf("pkt tr : %d\n",pkt_1->tr);
    printf("pkt_1 type : %d \n",pkt_1->type);
    printf("pkt_1 window : %d \n",pkt_1->tr);
    printf("pkt_1 length : %d \n",pkt_1->length);
    printf("pkt_1 timestamp : %d \n",pkt_1->timestamp);
    printf("pkt_1 seqnum : %d \n",pkt_1->seqnum);
    printf("pkt_1 payload : %s \n",pkt_1->payload);
    size_t length_header = predict_header_length(pkt_1);
    length_header = length_header + 8 + sizeof(buf);
    char *buffer = malloc(length_header);
    if ((pkt_encode(pkt_1,buffer,&length_header)) == PKT_OK){
        printf("Le paquet a bien été encodé\n");
    }
    else{
        printf("Erreur d'encodage du paquet\n");
    }
    pkt_t * pkt_test = pkt_new();
    if ((pkt_decode(buffer,length_header,pkt_test)) == PKT_OK){
        printf("Le paquet a bien été décodé\n");
    }
    else{
        printf("Erreur de décodage du paquet\n");
    }
    printf("Paquet 1 après encodage et décodage : \n");
    printf("pkt tr : %d\n",pkt_test->tr);
    printf("pkt_test type : %d \n",pkt_test->type);
    printf("pkt_test window : %d \n",pkt_test->tr);
    printf("pkt_test length : %d \n",pkt_test->length);
    printf("pkt_test timestamp : %d \n",pkt_test->timestamp);
    printf("pkt_test seqnum : %d \n",pkt_test->seqnum);
    printf("pkt_test payload : %s \n",pkt_test->payload);

    pkt_t * pkt_2 = pkt_new();
    pkt_set_tr(pkt_2,0);
    pkt_set_type(pkt_2,1);
    pkt_set_window(pkt_2,31);
    pkt_set_length(pkt_2,130);
    pkt_set_timestamp(pkt_2,127);
    pkt_set_seqnum(pkt_2,4);
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
    printf("Paquet 2 avant l'encodage : \n");
    printf("pkt tr : %d\n",pkt_2->tr);
    printf("pkt_2 type : %d \n",pkt_2->type);
    printf("pkt_2 window : %d \n",pkt_2->tr);
    printf("pkt_2 length : %d \n",pkt_2->length);
    printf("pkt_2 timestamp : %d \n",pkt_2->timestamp);
    printf("pkt_2 seqnum : %d \n",pkt_2->seqnum);
    printf("pkt_2 payload : %s \n",pkt_2->payload);
    size_t length_header_2 = predict_header_length(pkt_2);
    length_header_2 = length_header_2 + 8 + pkt_2->length;
    printf("sizeof :%lu buf_2\n",sizeof(buf_2));
    printf("Size %ld\n",length_header_2);
    char *buffer_2 = malloc(length_header_2);
    if ((pkt_encode(pkt_2, buffer_2, &length_header_2)) == PKT_OK){
        printf("Le paquet a bien été encodé\n");    }
    else{
        printf("Erreur d'encodage du paquet\n");
    }
    pkt_t * pkt_test_2 = pkt_new();
    if ((pkt_decode(buffer_2, length_header_2, pkt_test_2)) == PKT_OK){
        printf("Le paquet a bien été décodé\n");
    }
    else{
        printf("Erreur de décodage du paquet\n");
    }
    printf("Paquet 2 après encodage et décodage : \n");
    printf("pkt tr : %d\n",pkt_test_2->tr);
    printf("pkt_test_2 type : %d \n",pkt_test_2->type);
    printf("pkt_test_2 window : %d \n",pkt_test_2->tr);
    printf("pkt_test_2 length : %d \n",pkt_test_2->length);
    printf("pkt_test_2 timestamp : %d \n",pkt_test_2->timestamp);
    printf("pkt_test_2 seqnum : %d \n",pkt_test_2->seqnum);
    printf("pkt_test_2 payload : %s \n",pkt_test_2->payload);
}

int main(){
    printf("Début des tests sur la linked list ordonnée\n");
    test_create_delete_ordered_ll();
    printf("\n\n");
    printf("début des tests sur l'encodage des acks\n");
    testAckEncode();
    printf("\n\n");
    printf("Début des tests sur l'ajout dans le buffer de réception.\n");
    testAdd();
    printf("\n\n");
    printf("Début des tests sur la récupération noeuds d'une linked list\n");
    testRetrieve();
    printf("\n\n");
    printf("Début des tests sur la vérification d'un paquet. Cette fonction indique ce que l'on doit faire avec un paquet que l'on a reçu\n");
    testPktVerif();
    printf("\n\n");
    printf("Début des tests sur l'encodage et le décodage des paquets\n\n");
    testPacketImplem();
    return EXIT_SUCCESS;
}
