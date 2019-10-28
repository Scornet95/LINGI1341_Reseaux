#include "utils.h"

struct param_t getArguments(int argc, char* argv[]){
    struct param_t toRet;
    toRet.maxCo = 100;
    toRet.format = malloc(strlen("%d") + 1);
    strcpy(toRet.format, "%d");
    struct sockaddr_in6 host_adress;
    int opt, index;
    while ((opt = getopt(argc, argv, "m:o:")) != -1) {
		switch (opt) {
			case 'm':
				toRet.maxCo = atoi(optarg);
				break;
			case 'o':
                toRet.format = realloc(toRet.format, sizeof(char)*(strlen(optarg)+1));
                strcpy(toRet.format, optarg);
				break;
			default:
				fprintf(stderr, "Usage:\n"
								"-m     Maximum number of simultaneous connections.\n"
								"-o     Format of the output files\n"
								"Adress on which the receiver shall listen\n"
                                "Port on which the receiver shall listen\n");
				break;
		}
	}
    for(index = optind; index < argc; index++){
        const char *s = real_address(argv[index], &host_adress);
        if(s == NULL){
            toRet.address = malloc(sizeof(host_adress));
            memcpy(toRet.address,&host_adress,sizeof(host_adress));
        }
        else{
            toRet.port = atoi(argv[index]);
        }
    }
    return toRet;
}

const char * real_address(const char *address, struct sockaddr_in6 *rval){
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // output of getaddrinfo

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; // only IPV6
    hints.ai_socktype = SOCK_DGRAM; // tcp
    hints.ai_protocol = IPPROTO_UDP; // udp protocol

    status = getaddrinfo(address, NULL, &hints, &servinfo);
    if(status != 0){
        const char* error = gai_strerror(status);
        return error;
    }

    if(servinfo->ai_addr != NULL){
        memcpy(rval, servinfo->ai_addr, sizeof(struct sockaddr_in6));
    }

    freeaddrinfo(servinfo);
    return NULL;
}

int pkt_verif(pkt_t *pkt, int last_ack, int window){
    if (pkt_get_tr(pkt) == 1){
        return 1;
    }
    if (pkt_get_seqnum(pkt) == last_ack){
        return 0;
    }
    if(last_ack + window > 255){
        int overflow = (last_ack + window) % 256;
        if(pkt_get_seqnum(pkt) > last_ack || pkt_get_seqnum(pkt) < overflow){ //Le seqnum est dans la window
            return 3;
        }
        else if(pkt_get_seqnum(pkt) < last_ack)
            return 2;
        else
            return -1;
    }
    if (pkt_get_seqnum(pkt) < last_ack){
        return 2;
    }
    if (pkt_get_seqnum(pkt) > last_ack && pkt_get_seqnum(pkt) < last_ack + window){
        return 3;
    }
    else{
        fprintf(stderr,"Impossible to treat the sequence number\n");
        return -1;
    }
}

int create_socket(struct sockaddr_in6 *source_addr, int src_port, struct sockaddr_in6 *dest_addr, int dst_port){
    int sockfd;
    sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0){
        return -1;
    }
    if(source_addr != NULL && src_port > 0){
        source_addr->sin6_port = htons(src_port);
        source_addr->sin6_family = AF_INET6;
        int err = bind(sockfd,(struct sockaddr*) source_addr, sizeof(struct sockaddr_in6));
        if (err < 0){
            fprintf(stderr, "error while binding the socket to source adress\n");
            return -1;
        }
    }
    else if(dest_addr != NULL && dst_port > 0){
        dest_addr->sin6_port = htons(dst_port);
        dest_addr->sin6_family = AF_INET6;
        int err = connect(sockfd,(struct sockaddr*) dest_addr, sizeof(struct sockaddr_in6));
        if (err < 0){
            fprintf(stderr, "error while connecting the socket to dest\n");
            return -1;
        }
    }
    else{
        fprintf(stderr, "No valid source or dest addr/port\n");
        return -1;
    }
    return sockfd;
}

pkt_t* ackEncode(uint8_t seqnum, uint32_t timestamp, int ack, uint8_t window){
    pkt_t *pkt_ret;
    pkt_ret = pkt_new();
    if(ack){
        pkt_set_type(pkt_ret, 2);
        pkt_set_tr(pkt_ret, 0);
        pkt_set_window(pkt_ret, window);
        pkt_set_length(pkt_ret,0);
        pkt_set_seqnum(pkt_ret, seqnum);
        pkt_set_timestamp(pkt_ret, timestamp);
        pkt_ret->payload = NULL;
        return pkt_ret;
    }
    else{
        pkt_set_type(pkt_ret, 3);
        pkt_set_tr(pkt_ret, 0);
        pkt_set_window(pkt_ret, window);
        pkt_set_length(pkt_ret, 0);
        pkt_set_seqnum(pkt_ret, seqnum);
        pkt_set_timestamp(pkt_ret, timestamp);
        pkt_ret->payload = NULL;
        return pkt_ret;
    }
    return NULL;
}

void printPkt(pkt_t* pkt){
    if(pkt == NULL){
        printf("packet is NULL, nothing to print\n");
        return;
    }
    printf("type : %u\n", pkt_get_type(pkt));
    printf("tr : %u\n", pkt_get_tr(pkt));
    printf("window : %u\n", pkt_get_window(pkt));
    printf("length : %u\n", pkt_get_length(pkt));
    printf("seqnum : %u\n", pkt_get_seqnum(pkt));
    printf("timestamp : %u\n", pkt_get_timestamp(pkt));
    const char* payload = pkt_get_payload(pkt);
    printf("payload : \n");

    for(int i = 0; i < pkt_get_length(pkt); i++){
        printf("%c", *(payload + i));
    }
    printf("\n");
}
