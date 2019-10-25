#include "receiver.h"

int main(int argc, char* argv[]){
    struct param_t args = getArguments(argc, argv);

    /* Initialiser la sdd contenant les adresses de chaque sender, à faire pour plusieurs connections*/
    int sfd = create_socket(args.address, args.port, NULL, -1);
    if(sfd < 0){
        printf("error socket create\n");
        return -1;
    }
    struct sockaddr_in6 src_addr;
    socklen_t length = (socklen_t) sizeof(struct sockaddr_in6);

    char* firstBuffer = malloc(sizeof(char) * MAX_PACKET_SIZE);
    if(firstBuffer == NULL)
        return -1;

    address_t addie;
    addie.last_ack = 0;
    addie.buffer = create_ordered_ll();
    addie.window = 31;
    addie.fd = open("zeb", O_CREAT | O_APPEND | O_WRONLY | O_TRUNC);
    addie.acks = create_ordered_ll();

    int status;

    struct pollfd pfd[1];
    ssize_t size;
    while(1){
        pfd[0].fd = sfd;
        pfd[0].events = POLLIN | POLLOUT;

        poll(pfd, 1, -1);
        if(pfd[0].revents & POLLIN){ //On a reçu un paquet donc il faut le traiter.
            size = recvfrom(sfd, firstBuffer, (size_t) MAX_PACKET_SIZE, 0, (struct sockaddr*) &src_addr, &length); //Cet appel permet de récupérer l'adresse du sender.
            /*Regarder dans la table des adresses si on connaît cette adresse ci et déterminer ce qu'on fait avec les données.*/

            addie.address = malloc(length);
            memcpy(addie.address, &src_addr, length);
            pkt_t* pkt = pkt_new();
            pkt_status_code err = pkt_decode(firstBuffer, size, pkt);
            printf(" pkt decode timestamp %u\n",pkt_get_timestamp(pkt));
            if(err != PKT_OK){
                printf("err : %d\n", err);
                pkt_del(pkt);
            }

            status = pkt_verif(pkt, addie.last_ack, addie.window);
            if(status == 0){ //Le paquet reçu correspond à celui attendu, on le place dans le buffer, on update last_ack et on renvoie un ack.
                printf("stat0\n");
                add(addie.buffer, pkt, addie.last_ack);
                addie.timestamp = pkt_get_timestamp(pkt);
                printf("addie timestamp : %u\n", addie.timestamp);
            }

            else if(status == 1){ //Le paquet reçu est tronqué, on encode un NACK.
                //encoder un NACK et le mettre dans la fifo.
                addie.timestamp = pkt_get_timestamp(pkt);
                pkt_t* nAck = ackEncode(pkt_get_seqnum(pkt), addie.timestamp, 0, (addie.window - addie.buffer->size));
                enqueue(addie.acks, nAck);
            }

            else if(status == 2){
                addie.timestamp = pkt_get_timestamp(pkt);
                pkt_t* ack = ackEncode(addie.last_ack, addie.timestamp, 1, (addie.window - addie.buffer->size));
                enqueue(addie.acks, ack);
            }

            else if(status == 3){
                printf("packet out of sequence\n");
                add(addie.buffer, pkt, addie.last_ack);
                addie.timestamp = pkt_get_timestamp(pkt);
                pkt_t* ack = ackEncode(addie.last_ack, addie.timestamp, 1, (addie.window - addie.buffer->size));
                enqueue(addie.acks, ack);
            }
            emptyBuffer(&addie);
            /*fonction pour vider le buffer et encoder un ack + écrire dans le fichier correspondant.*/
            pkt_del(pkt);
        }
        if(pfd[0].revents & POLLOUT){ //Il y a de la place pour écrire sur le socket, c'est ici que l'on va envoyer les acks.
            if(sendQueue(sfd, &addie) != 0)
                printf("erreur sendQueue\n");
        }
    }
}

int emptyBuffer(address_t* add){
    if(peek(add->buffer) > add->last_ack){
        printf("pas lastack\n");
        return -1;
    }
    else{//Le premier élément de la liste a le seqnum last_ack
        int maxSeq;
        ssize_t err;
        pkt_t* pkt;
        do{
            pkt = retrieve(add->buffer);
            if(pkt != NULL){
                maxSeq = pkt_get_seqnum(pkt);
                err = write(add->fd, pkt_get_payload(pkt), pkt_get_length(pkt));
                if(err < MAX_PAYLOAD_SIZE){ //Je suppose ici que si je reçois un paquet qui n'est pas de taille max, c'est que c'est le dernier et il contient donc eof
                    close(add->fd);
                    break;
                }
            }
        }while(peek(add->buffer) == (maxSeq + 1) % 256);
        //mettre last_ack à jour et encoder le ack que l'on va envoyer
        add->last_ack = maxSeq + 1;
        pkt_t* ack = ackEncode(add->last_ack, add->timestamp, 1, (add->window - add->buffer->size));
        enqueue(add->acks, ack);
    }
    return 0;
}

int sendQueue(int sockfd, address_t* addie){
    pkt_t* pkt;
    pkt_status_code err;
    char* buf = malloc(sizeof(char) * 11);
    size_t len = 11;
    while((addie->acks)->size > 0){
        printf("queue size : %d\n", (addie->acks)->size);
        pkt = retrieve(addie->acks);
        if(pkt != NULL){
             printf("paquet n %d\t timestamp %u\n", pkt_get_seqnum(pkt), pkt_get_timestamp(pkt));
             err = pkt_encode(pkt, buf, &len);
             if(err == PKT_OK){
                 sendto(sockfd, buf, len, 0, (struct sockaddr*) addie->address, sizeof(struct sockaddr_in6));
             }
             else{
                printf("erreur d'encode\n");
                return -1;
            }
        }
        else{
            printf("erreur retrieve\n");
            return -1;
        }
    free(buf);
    }
    return 0;
}
