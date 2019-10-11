#include "packet_interface.h"
#include <stdlib.h>

/* Extra #includes */
/* Your code will be inserted here */
#include <zlib.h>

int main(void){
    pkt_t* packet = pkt_new();
    printf("size of the packet : %ld\n", sizeof(packet));
    pkt_del(packet);
}


struct __attribute__((__packed__)) pkt {
    ptypes_t type;
    uint8_t tr;
    uint8_t window;
    uint8_t l;
    uint16_t length;
    uint8_t seqnum;
    uint32_t timestamp;
    uint32_t crc1;
    uint32_t crc2;
    char* payload;
};


/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t* toRet = malloc(sizeof(pkt_t));
    if(toRet == NULL)
        return NULL;
    return toRet;

}

void pkt_del(pkt_t *pkt)
{
    free(pkt->payload);
    free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    pkt = pkt_new();
    int index = 0;
    pkt->type = data[index] >> 6;
    if(pkt->type != 1 && pkt->type != 2 && pkt->type != 3){
        pkt_del(pkt);
        return E_TYPE;
    }
    pkt->tr = (data[index] << 2) >> 7;
    if(pkt->tr == 1 && pkt->type != PTYPE_DATA){
        pkt_del(pkt);
        return E_TR;
    }
    pkt->window = (data[index] << 3) >> 3;
    if(varuint_decode((data + 1), (ssize_t) 2, &(pkt->length)) == 1){
        pkt->l = 0;
        index = index + 2;
    }
    else{
        pkt->l = 1;
        index = index + 3;
    }
    pkt->seqnum = (uint8_t) data[index]; //test pour la window à faire mais je ne sais pas comment.
    index++;

    uint32_t timestamp = 0;
    for(int i = 0; i < 4; i++){
        timestamp += data[index];
        timestamp = timestamp << 8;
        index++;
    }
    pkt->timestamp = timestamp;

    uint32_t crc1 = 0;
    for(int i = 0; i < 4; i++){
        crc1 += data[index];
        crc1 = crc1 << 8;
        index++;
    }
    pkt->crc1 = ntohl(crc1);
    //Mtn on va recalculer le crc du header et le comparer avec celui reçu.
    uLong crc = crc32(0L, Z_NULL, 0);
    char* buf = malloc(sizeof(char));
    memcpy(buf, data, 1);
    *buf = *buf & 223;
    crc = crc32(crc, buf, 1);
    free(buf);
    int headLength = predict_header_length(pkt);
    if(pkt->tr == 1){
        if(len != headLength + 4){ // le header + le crc1
            pkt_del(pkt);
            return E_UNCONSISTENT;
        }
    }
    else{
        if(len != headLength + 8 + pkt->length){ // le header + les deux crc + la taille du payload.
            pkt_del(pkt);
            return E_UNCONSISTENT;
        }
    }
    for(int i = 1; i < headLength, i++){
        crc = crc32(crc, data + i, 1);
    }
    if(crc != pkt->crc1){
        pkt_del(pkt);
        return E_CRC;
    }
    if(pkt->type == PTYPE_ACK)
        return PKT_OK;
    if(pkt->tr == 0 && index == len){
        return PKT_OK;
    }
    else if(pkt->tr == 0 && index != len){
        pkt_del(pkt);
        return E_UNCONSISTENT;
    }
    char* payload = malloc(sizeof(char) * pkt->length);
    if(payload == NULL)
        return -1;
    for(int i = 0; i < pkt->length; i++, index++){
        payload[i] = data[index];
    }
    if(pkt_set_payload(pkt, payload, pkt->length) != PKT_OK){
        pkt_del(pkt);
        return -1;
    }
    free(payload);
    //Mtn on va calculer le crc2 du paquet, on sait que c'est un paquet de type data
    uint32_t crc2 = 0;
    for(int i = 0; i < 4; i++){
        crc2 += data[index];
        crc2 = crc2 << 8;
        index++;
    }
    pkt->crc2 = ntohl(crc2);
    crc = crc32(0L, Z_NULL, 0);
    index = index - 4 - pkt->length;
    for(int i = index; i < index + pkt->length; i++){
        crc = crc32(crc, data + i; 1);
    }
    if(crc != pkt->crc2){
        pkt_del(pkt);
        return E_CRC;
    }
    else
        return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{   if (len == 0){
      return E_NOMEM;
      }
    int index = 0;
    ptypes_t type_retrieve;
    type_retrieve = pkt_get_type(pkt);
    uint8_t type;
    if (type_retrieve == 1){
      type = 1;
    }
    if (type_retrieve == 2){
      type = 2;
    }
    if (type_retrieve == 3){
      type = 3;
    }
    uint8_t tr = pkt_get_tr(pkt);
    uint8_t window = pkt_get_window(pkt);
    uint8_t first8;
    first8 = (type << 6);
    first8 = first8 + (tr << 5);
    first8 = first8 + window;
    *buf = (char) first8;
    index += 1;
    uint16_t length = pkt_get_length(pkt);
    ssize_t l = varuint_predict_len(length);
    uint8_t * var_uint = (uint8_t *) malloc(l);
    if (var_uint == NULL){
      return E_NOMEM;
    }
    ssize_t err = varuint_encode(length, var_uint, l);
    if (err == -1){
      free(var_uint);
      return E_LENGTH;
    }
    *(buf + index) = (char) var_uint[0];
    if (err == 1){
      index += 1;
    }
    if (err == 2){
      index += 2;
    }
    if (index >= len){
      return E_NOMEM;
    }
    uint8_t seqnum = pkt_get_seqnum(pkt);
    *(buf + index) = (char) seqnum;
    index += 1;
    if (index >= len){
      return E_NOMEM;
    }



}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    pkt->payload = malloc(sizeof(char) * length);
    if(pkt->payload == NULL)
        E_NOMEM;
    memcpy(pkt->payload, data, length);
    return PKT_OK;
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    if(len != 1 && len != 2)
        return -1;
    if((*data >> 7) == 0){
        *retval = *data;
        return 1;
    }
    else{
        if(len < 2)
            return -1;
        *retval = ntohs((*data << 1) >> 1);
        return 2;
    }
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
{
    if(val > 127){ //alors val est sur 15 bits
        if(len < 2)
            return -1;
        *data = htons(val) | 32768;
        return 2;
    }
    else{
        if(len < 1)
            return -1;
        *data = val;
        return 1;
    }
}

size_t varuint_len(const uint8_t *data)
{
    if(*data >> 7 == 0)
        return 1;
    else
        return 2;
}


ssize_t varuint_predict_len(uint16_t val)
{
    if(val >= 0x8000)
        return -1;
    if(val > 127)
        return 2;
    else
        return 1;
}


ssize_t predict_header_length(const pkt_t *pkt)
{
    if(pkt->length >= 0x8000)
        return -1;
    if(pkt->length > 127)
        return (ssize_t) 8;
    else
        return (ssize_t) 7;
}
