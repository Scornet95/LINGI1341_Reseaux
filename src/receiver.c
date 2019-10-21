#include "receiver.h"

int main(int argc, char* argv[]){
    struct param_t args = getArguments(argc, argv);

    /* Initialiser la sdd contenant les adresses de chaque sender, à faire pour plusieurs connections*/

    int sfd = create_socket(args.adress, args.port, NULL, -1);
    if(sfd < 0){
        printf("error socket create\n");
        return -1;
    }

    struct sockaddr_in6 src_addr;
    socklen_t length = (socklen_t) sizeof(struct sockaddr_in6);

    char* firstBuffer = malloc(sizeof(char) * MAX_PACKET_SIZE);
    if(firstBuffer == NULL)
        return -1;
    char* ackBuffer = malloc(sizeof(char) * 7);
    if(ackBuffer == NULL)
        return -1;

    address_t addie;
    addie.last_ack = 0;
    addie.buffer = create_ordered_ll();

    int firstPass = 0;
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


            pkt_t* pkt = pkt_new();
            pkt_status_code err = pkt_decode(firstBuffer, size, pkt);
            if(err != PKT_OK){
                printf("err : %d\n", err);
                pkt_del(pkt);
            }

            printf("window du premier paquet : %u\n", pkt->window);
            return 0;

            if(firstPass == 0){
                addie.adress = &src_addr;
                addie.window = pkt_get_window(pkt);
            }

            status = pkt_verif(pkt, addie.last_ack);
            if(status == 0){ //Le paquet reçu correspond à celui attendu, on le place dans le buffer, on update last_ack et on renvoie un ack.
                    
            }

            pkt_del(pkt);


        }
        if(pfd[0].revents & POLLOUT){ //Il y a de la place pour écrire sur le socket, c'est ici que l'on va envoyer les acks.
            printf("ack\n");
        }

    }
}
