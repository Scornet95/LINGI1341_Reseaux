#include "utils.h"
#include "packet_interface.h"
struct param_t getArguments(int argc, char* argv[]){
    struct param_t toRet;
    struct sockaddr_in6 host_adress;
    int optind, index;
    while ((optind = getopt(argc, argv, "m:o:")) != -1) {
		switch (optind) {
			case 'm':
				toRet.maxCo = atoi(optarg);
				break;
			case 'o':
                toRet.format = malloc(sizeof(char)*(strlen(optarg)+1));
				toRet.format = optarg;
				break;
			default:
				fprintf(stderr, "Usage:\n"
								"-s      Act as server\n"
								"-c      Act as client\n"
								"-p PORT UDP port to connect to (client)\n"
								"        or to listen on (server)\n"
								"-h HOST UDP of the server (client)\n"
								"        or on which we listen (server)\n");
				break;
		}
	}
    for(index = optind; index < argc; index++){
        const char *s = real_address(argv[index], &host_adress);
        if(s == NULL){
            toRet.adress = malloc(sizeof(host_adress));
            memcpy(toRet.adress,&host_adress,sizeof(host_adress));
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
int pkt_verif(pkt_t *pkt, int last_ack){
    if (pkt_get_tr(pkt) == 1){
        return 1;
    }
    if (pkt_get_seqnum(pkt) < last_ack){
        return 2;
    }
    if (pkt_get_seqnum(pkt) > last_ack){
        return 3;
    }
    if (pkt_get_seqnum(pkt) == last_ack){
        return 0;
    }
    else{
        fprintf(stderr,"Impossible to treat the sequence number\n");
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

char * pkt_nack_or_ack(int verif, int * last_ack, pkt_t check_seqnum, int places_buffer){
    pkt_t *pkt_ret = pkt_new();
    char *buf = malloc(sizeof(char)*7);
    uint32_t crc1;
    if (verif == 1){
        pkt_ret->type = 3;
        pkt_ret->tr = 0;
        pkt_ret->window = places_buffer;
        pkt_ret->length = 0;
        pkt_ret->seqnum = check_seqnum.seqnum;
        pkt_ret->timestamp = check_seqnum.timestamp;
        if ((pkt_encode(pkt_ret, buf, 7)) == PKT_OK{
            return buf;
        }
        else{
            fprintf(stderr,"Error while encoding the nack response\n");
        }
    }
    if (verif == 2 || verif == 3){
        pkt_ret->type = 2;
        pkt_ret->tr = 0;
        pkt_ret->window = places_buffer;
        pkt_ret->length = 0;
        pkt_ret->seqnum = last_ack;
        pkt_ret->timestamp = check_seqnum.timestamp;
        if ((pkt_encode(pkt_ret, buf, 7)) == PKT_OK{
            return buf;
        }
        else{
            fprintf(stderr,"Error while encoding the ack response with different sequence numbers\n");
        }
    }
    if (verif == 0){
        pkt_ret->type = 2;
        pkt_ret->tr = 0;
        pkt_ret->window = places_buffer;
        pkt_ret->length = 0;
        *last_ack += 1;
        pkt_ret->seqnum = last_ack;
        pkt_ret->timestamp = check_seqnum.timestamp;
        if ((pkt_encode(pkt_ret, buf, 7)) == PKT_OK{
            return buf;
        }
        else{
            fprintf(stderr,"Error while encoding the ack response for same sequence number\n");
        }
    }


}
