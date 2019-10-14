#include "packet_interface.h"
#include <stdlib.h>

/* Extra #includes */
/* Your code will be inserted here */
#include <zlib.h>
#include <arpa/inet.h>
#include <string.h>

struct __attribute__((__packed__)) pkt {
    ptypes_t type;
    uint8_t tr;
    uint8_t window;
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
    pkt_set_type(pkt, data[index] >> 6);
    if(pkt->type != 1 && pkt->type != 2 && pkt->type != 3){
        pkt_del(pkt);
        return E_TYPE;
    }
    pkt_set_tr(pkt, (data[index] << 2) >> 7);
    if(pkt->tr == 1 && pkt->type != PTYPE_DATA){
        pkt_del(pkt);
        return E_TR;
    }
    pkt_set_window(pkt, (data[index] << 3) >> 3);
    if(varuint_decode((uint8_t*)(data + 1), (ssize_t) 2, &(pkt->length)) == 1){
        index = index + 2;
    }
    else{
        index = index + 3;
    }
    pkt->seqnum = (uint8_t) data[index]; //test pour la window à faire mais je ne sais pas comment.
    index++;

    uint32_t timestamp = 0;
    int i;
    for(i = 0; i < 4; i++){
        timestamp += data[index];
        timestamp = timestamp << 8;
        index++;
    }
    pkt->timestamp = timestamp;

    uint32_t crc1 = 0;
    for(i = 0; i < 4; i++){
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
    crc = crc32(crc, (Bytef*) buf, 1);
    free(buf);
    size_t headLength = predict_header_length(pkt);
    if(pkt->tr == 1){
        if(len != headLength + (ssize_t) 4){ // le header + le crc1
            pkt_del(pkt);
            return E_UNCONSISTENT;
        }
    }
    else{
        if(len != headLength + (ssize_t) 8 + (ssize_t) pkt->length){ // le header + les deux crc + la taille du payload.
            pkt_del(pkt);
            return E_UNCONSISTENT;
        }
    }
    for(i = 1; i < (int) headLength; i++){
        crc = crc32(crc,(Bytef*) data + i, 1);
    }
    if(crc != pkt->crc1){
        pkt_del(pkt);
        return E_CRC;
    }
    if(pkt->type == PTYPE_ACK)
        return PKT_OK;
    if(pkt->tr == 0 && index == (int) len){
        return PKT_OK;
    }
    else if(pkt->tr == 0 && index != (int) len){
        pkt_del(pkt);
        return E_UNCONSISTENT;
    }
    char* payload = malloc(sizeof(char) * pkt->length);
    if(payload == NULL)
        return -1;
    for(i = 0; i < pkt->length; i++, index++){
        payload[i] = data[index];
    }
    if(pkt_set_payload(pkt, payload, pkt->length) != PKT_OK){
        pkt_del(pkt);
        return -1;
    }
    free(payload);
    //Mtn on va calculer le crc2 du paquet, on sait que c'est un paquet de type data
    uint32_t crc2 = 0;
    for(i = 0; i < 4; i++){
        crc2 += data[index];
        crc2 = crc2 << 8;
        index++;
    }
    pkt->crc2 = ntohl(crc2);
    crc = crc32(0L, Z_NULL, 0);
    index = index - 4 - pkt->length;
    for(i = index; i < index + pkt->length; i++){
        crc = crc32(crc, (Bytef*) data + i, 1);
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
      index += 1;
      if (index >= (int) *len){
        return E_NOMEM;
      }
      *(buf + index) = (char) var_uint[1];
      index += 1;

    }
    if (index >= (int) *len){
      return E_NOMEM;
    }
    uint8_t seqnum = pkt_get_seqnum(pkt);
    *(buf + index) = (char) seqnum;
    index += 1;
    if (index >= (int) *len){
      return E_NOMEM;
    }
    uint32_t timestamp = pkt_get_timestamp(pkt);
    uint8_t * timestamp_8bit = (uint8_t *) malloc(4);
    timestamp_8bit[0] = timestamp >> 24;
    timestamp_8bit[1] = (timestamp >> 16) & 0x000000ff;
    timestamp_8bit[2] = (timestamp >> 8) & 0x000000ff;
    timestamp_8bit[3] = (timestamp & 0x000000ff);
    int i;
    for (i = 0; i < 4; i+=1){
      *(buf + index) = (char) timestamp_8bit[i];
      index += 1;
      if (index >= (int) *len){
        free(timestamp_8bit);
        return E_NOMEM;
      }
    }
    free(timestamp_8bit);
    //Calcul of the first crc
    uint32_t crc_1 = crc32(0L, Z_NULL, 0);
    char* tr_to_0 = (char *) malloc(sizeof(char));
    memcpy(tr_to_0,buf,1);
    *tr_to_0 = *tr_to_0 & 223;
    crc_1 = crc32(crc_1, (const Bytef *) tr_to_0, 1);
    ssize_t headLength = predict_header_length(pkt);
    int j;
    for (j = 1; j < headLength; j+=1){
      crc_1 = crc32(crc_1, (const Bytef *) buf + j, 1);
    }
    uint8_t * crc = (uint8_t *) malloc(4);
    crc[0] = crc_1 & 0x000000ff;
    crc[1] = (crc_1 >> 8) & 0x000000ff;
    crc[2] = (crc_1 >> 16) & 0x000000ff;
    crc[3] = crc_1 >> 24;
    int v;
    for (v = 0; v < 4; v+=1){
      *(buf + index) = (char) crc[v];
      index += 1;
      if (index >= (int) *len){
        free(crc);
        return E_NOMEM;
      }
    }
    free(crc);
    int index_calcul_crc2 = index;
    //Payload in the buffer
    if (type == 1){
        const char * payload = pkt_get_payload(pkt);
        int k;
        for(k = 0; k < length; k+=1){
          *(buf + index) = payload[k];
          index += 1;
          if(index >= (int) *len){
            return E_NOMEM;
          }
        }
    }
    else{
      return PKT_OK;
    }

    //Calcule du crc2
    uint32_t crc_2 = crc32(0L,Z_NULL,0);
    for( ; index_calcul_crc2 < length; index_calcul_crc2+=1){
      crc_2 = crc32(crc_2, (Bytef*) (buf+index_calcul_crc2),1);
    }
    uint8_t* crc_2_final = (uint8_t *) malloc(sizeof(uint8_t)*2);
    crc_2_final[0] = (crc_2) & 0x000000ff;
    crc_2_final[1] = (crc_2 >> 8) & 0x000000ff;
    crc_2_final[2] = (crc_2 >> 16) & 0x000000ff;
    crc_2_final[3] = crc_2 >> 24;
    int o;
    for (o = 0; o < 4; o+=1){
      *(buf + index) = (char) crc_2_final[o];
      index += 1;
      if (index >= (int) *len){
        free(crc_2_final);
        return E_NOMEM;
      }
    }
    free(crc_2_final);
    return PKT_OK;
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
    return pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return pkt->crc1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    pkt->type = type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    pkt->tr = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    pkt->window = window;
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    pkt->seqnum = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    pkt->length = length;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->timestamp = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->crc1 = crc1;
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    pkt->crc2 = crc2;
    return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    pkt->payload = malloc(sizeof(char) * length);
    if(pkt->payload == NULL)
        return E_NOMEM;
    memcpy(pkt->payload, data, length);
    return PKT_OK;
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    if(len != 1 && len != 2)
        return -1;
    if(varuint_len(data) == 1){
        *retval = (uint16_t) *data;
        return 1;
    }
    else{
        *retval = (*data & 127) << 8;
        *retval += *(data + 1);
        *retval = ntohs(*retval);
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
