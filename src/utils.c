#include "utils.h"

struct param_t getArguments(int argc, char* argv[]){
    struct param_t toRet;
    int opt, index;
    while ((opt = getopt(argc, argv, "m:o:")) != -1) {
		switch (opt) {
			case 'm':
				toRet.maxCo = atoi(optarg);
				break;
			case 'o':
				toRet.format = optarg;
				break;
			default:
				fprintf(stderr, "");
				break;

		}
	}
    for(index = optind; index < argc; idex++){

    }
}

const char * real_address(const char *address, struct sockaddr_in6 *rval){
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // output of getaddrinfo

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; // only IPV6
    hints.ai_socktype = SOCK_DGRAM; // tcp
    hints.ai_protocol = IPPROTO_UDP; // udp protocol
    hints.ai_flags = AI_PASSIVE; // fill in my Ip for me

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
int pkt_verif(pkt_t *pkt,int last_ack){
    if (pkt->tr == 1){
        return 1;
    }
    if (pkt->seqnum < last_ack){
        return 2;
    }
    if (pkt->seqnum > last_ack){
        return 3;
    }
    if (pkt->seqnum == last_ack){
        return 0;
    }
}

int create_socket(struct sockaddr_in6 *source_addr,int src_port,struct sockaddr_in6 *dest_addr,int dst_port){
    int sockfd;
    sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
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
