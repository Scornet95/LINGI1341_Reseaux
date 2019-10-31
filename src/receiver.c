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

    struct linked_list* senders = create_linked_list();
    ackQueue* acks = createAckQueue();
    int count = 0;

    address_t* addie;

    int status;

    struct pollfd pfd[1];
    ssize_t size;
    pkt_t* pkt;

    while(1){
        pfd[0].fd = sfd;
        pfd[0].events = POLLIN | POLLOUT;

        poll(pfd, 1, -1);
        if(pfd[0].revents & POLLIN){ //On a reçu un paquet donc il faut le traiter.
            size = recvfrom(sfd, firstBuffer, (size_t) MAX_PACKET_SIZE, 0, (struct sockaddr*) &src_addr, &length); //Cet appel permet de récupérer l'adresse du sender.
            //Regarder dans la table des adresses si on connaît cette adresse ci et déterminer ce qu'on fait avec les données.
            pkt = pkt_new();
            pkt_status_code err = pkt_decode(firstBuffer, size, pkt);

            if(err == PKT_OK){
                addie = search_linked_list(senders, &src_addr, args.maxCo, args.format, &count, pkt_get_length(pkt));
            }
            else
                addie = NULL;

            if(addie == NULL){
                if(err == PKT_OK){
                    if(pkt_get_length(pkt) == 0){
                        pkt_t* ack = ackEncode((pkt_get_seqnum(pkt) + 1) % 256, pkt_get_timestamp(pkt), 1, 31);
                        enqueue_ack_queue(ack, &src_addr, acks);
                    }
                }
                else
                    pkt_del(pkt);
            }
            else{
                status = pkt_verif(pkt, addie->last_ack, addie->window);
                if(err != PKT_OK){
                     ;
                }
                else if(status == 0){ //Le paquet reçu correspond à celui attendu, on le place dans le buffer.
                    add(addie->buffer, pkt, addie->last_ack);
                }

                else if(status == 1){ //Le paquet reçu est tronqué, on encode un NACK.
                    pkt_t* nAck = ackEncode(pkt_get_seqnum(pkt), pkt_get_timestamp(pkt), 0, (addie->window - addie->buffer->size));
                    enqueue(addie->acks, nAck);
                }

                else if(status == 2){
                    pkt_t* ack = ackEncode(addie->last_ack, pkt_get_timestamp(pkt), 1, (addie->window - addie->buffer->size));
                    enqueue(addie->acks, ack);
                }

                else if(status == 3){
                    add(addie->buffer, pkt, addie->last_ack);
                }
                if(emptyBuffer(addie, acks) == 1){
                    remove_linked_list(senders, addie);
                }
            }
        }
        if(pfd[0].revents & POLLOUT){ //Il y a de la place pour écrire sur le socket, c'est ici que l'on va envoyer les acks.
            address_node* runner = senders->front;
            while(runner != NULL){
                if(sendQueue(sfd, runner->sender_address) != 0)
                    printf("erreur sendQueue!\n");
                runner = runner->next;
            }
            if(sendAckQueue(sfd, acks) != 0)
                printf("erruer en envoyant les acks tu conné\n");
        }
    }
}
