#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include"real_address.h"

const char * real_address(const char *address, struct sockaddr_in6 *rval){
  int errcode;
  const char * erreur_string;
  char * erreur_du_rval;
  struct addrinfo hints;
  struct addrinfo *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags |= AI_CANONNAME;
  errcode = getaddrinfo(address, NULL, &hints, result);
  if (errcode < 0){
    *erreur_string = gai_strerror(errcode);
    return *erreur_string;
  }
  struct sockaddr_in6 *addresse = (struct sockaddr_in6) malloc(sizeof(struct sockaddr_in6));
  addresse = result->ai_addr;
  *rval = *addresse;
  if (rval != NULL){
    freeaddrinfo(result);
    return NULL;
  }
  else{
    freeaddrinfo(result);
    *erreur_du_rval = "L'adresse n'existe pas";
    return erreur_du_rval;
  }
}
