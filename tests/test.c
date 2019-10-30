#include "packet_interface.h"
#include <stdlib.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <string.h>

int main(){
    printf("Début des test concernant l'implémentation des paquets\n\n");
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
    printf("Size %d\n",length_header_2);
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
