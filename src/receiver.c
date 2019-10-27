#include "receiver.h"

int main(int argc, char* argv[]){
    struct param_t args = getArguments(argc, argv);

    // Initialiser la sdd contenant les adresses de chaque sender, à faire pour plusieurs connections
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

    address_t connections[args.maxCo];
    int numberOfCo = 0;
    int count = 0;

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
            //Regarder dans la table des adresses si on connaît cette adresse ci et déterminer ce qu'on fait avec les données.

            addie.address = malloc(length);
            memcpy(addie.address, &src_addr, length);
            pkt_t* pkt = pkt_new();
            pkt_status_code err = pkt_decode(firstBuffer, size, pkt);
            if(err != PKT_OK){
                printf("err : %d\n", err);
                pkt_del(pkt);
            }

            status = pkt_verif(pkt, addie.last_ack, addie.window);
            if(status == 0){ //Le paquet reçu correspond à celui attendu, on le place dans le buffer.
                add(addie.buffer, pkt, addie.last_ack);
            }

            else if(status == 1){ //Le paquet reçu est tronqué, on encode un NACK.
                //encoder un NACK et le mettre dans la fifo.
                pkt_t* nAck = ackEncode(pkt_get_seqnum(pkt), pkt_get_timestamp(pkt), 0, (addie.window - addie.buffer->size));
                enqueue(addie.acks, nAck);
            }

            else if(status == 2){
                pkt_t* ack = ackEncode(addie.last_ack, pkt_get_timestamp(pkt), 1, (addie.window - addie.buffer->size));
                enqueue(addie.acks, ack);
            }

            else if(status == 3){
                add(addie.buffer, pkt, addie.last_ack);
                pkt_t* ack = ackEncode(addie.last_ack, pkt_get_timestamp(pkt), 1, (addie.window - addie.buffer->size));
                enqueue(addie.acks, ack);
            }
            emptyBuffer(&addie);
            //fonction pour vider le buffer et encoder un ack + écrire dans le fichier correspondant.
        }
        if(pfd[0].revents & POLLOUT){ //Il y a de la place pour écrire sur le socket, c'est ici que l'on va envoyer les acks.
            if(sendQueue(sfd, &addie) != 0)
                printf("erreur sendQueue\n");
        }
    }
}


int emptyBuffer(address_t* add){
    if(peek(add->buffer) == add->last_ack){
        int maxSeq;
        pkt_t* pkt, *ack;
        do{
            pkt = retrieve(add->buffer);
            if(pkt != NULL){
                maxSeq = pkt_get_seqnum(pkt);
                if(maxSeq == add->last_ack && pkt_get_length(pkt) == 0){
                    close(add->fd);
                    ack = ackEncode((maxSeq + 1) % 256, pkt_get_timestamp(pkt), 1, (add->window - add->buffer->size));
                    enqueue(add->acks, ack);
                    return 1;
                }
                write(add->fd, pkt_get_payload(pkt), pkt_get_length(pkt));
                ack = ackEncode((maxSeq + 1) % 256, pkt_get_timestamp(pkt), 1, (add->window - add->buffer->size));
                enqueue(add->acks, ack);
            }
        }while(peek(add->buffer) == (maxSeq + 1) % 256);
        //mettre last_ack à jour et encoder le ack que l'on va envoyer
        add->last_ack = (maxSeq + 1) % 256;
        pkt_del(pkt);
    }
    else{//Le premier élément de la liste a le seqnum last_ack
        return -1;
    }
    return 0;
}

int sendQueue(int sockfd, address_t* addie){
    pkt_t* pkt;
    pkt_status_code err;
    char* buf = malloc(sizeof(char) * 11);
    size_t len = 11;
    while((addie->acks)->size > 0){
        pkt = retrieve(addie->acks);
        if(pkt != NULL){
             err = pkt_encode(pkt, buf, &len);
             if(err == PKT_OK){
                 sendto(sockfd, buf, len, 0, (struct sockaddr*) addie->address, sizeof(struct sockaddr_in6));
             }
             else{
                printf("erreur d'encode\n");
                free(buf);
                return -1;
            }
        }
        else{
            printf("erreur retrieve\n");
            free(buf);
            return -1;
        }
    }
    free(buf);
    return 0;
}
