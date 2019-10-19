#include "utils.h"

struct param_t getArguments(int argc, char* argv[]){
    struct param_t toRet;
    struct sockaddr_in6 host_adress;
    int opt, index;
    while ((opt = getopt(argc, argv, "m:o:")) != -1) {
		switch (opt) {
			case 'm':
				toRet.maxCo = atoi(optarg);
				break;
			case 'o':
                toRet.format = malloc(sizeof(char)*(strlen(optarg)+1);
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
    if (argc > optind){
        fprintf(stderr,"wrong numbers of arguments");
    }
    if ((real_address(argv[optind],&host_adress)) != NULL){
        fprintf(stderr, "The function real_adress failed");
        }
    toRet.adress = malloc(sizeof(host_adress));
    memcpy(toRet.adress,&host_adress,sizeof(host_adress));
    index++;
    if (argc > optind){
        fprintf(stderr,"wrong numbers of arguments");
    }
    toRet.port = argv[optind];
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
