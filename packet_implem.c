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
    void* payload;
};


/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t* toRet = malloc(sizeof(pkt_t));
    if(toRet == NULL)
        return NULL;
    toRet->payload = malloc(sizeof(char) * 512);
    if(toRet->payload == NULL)
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
    uint8_t* first8 = data;
    ptypes_t type = *first8 >> 6;
    if(type == 0){
        pkt_del(pkt);
        return E_TYPE;
    }
    pkt->type = type;
    pkt->tr = (*first8 << 2) >> 7;
    if(pkt->tr == 1 && pkt->type != 1){
        pkt_del(pkt);
        return E_TR;
    }

    pkt->window = (*first8 << 3) >> 3;
    uint8_t* second8 = data + 1;
    pkt->l = *second8 >> 7;
    uint8_t* seqnum;
    if(pkt->l == 0){
        pkt->length = *second8;
        seqnum = second8 + 1;
        pkt->seqnum = *seqnum;
    }
    else{
        uint16_t* second16 = second8;
        pkt->length = ntohs((*second16 << 1) >> 1);
        if(pkt->length > 512){
            pkt_del(pkt);
            return E_LENGTH;
        }
        seqnum = second8 + 2;
        pkt->seqnum = *seqnum;
    }
    int* timestamp = seqnum + 1;
    pkt->timestamp = *timestamp;
    int* crc1 = timestamp + 1;
    pkt->crc1 = ntohs(*crc1);
    //On recalcule le crc sur le header et on compare avec celui que l'on a reçu.
    unsigned long long *header = data;
    unsigned long long header2 = *header & 0b1101111111111111111111111111111111111111111111111111111111111111;
    //Petit and logique pour passer le champ tr à 0
    if(pkt->l == 0){
        uLong crc = crc32(0L, &header2, 7);
        if(crc != pkt->crc1){
            pkt_del(pkt);
            return E_CRC;
        }
    }
    else{
        uLong crc = crc32(0L, &header2, 8);
        if(crc != pkt->crc1){
            pkt_del(pkt);
            return E_CRC;
        }
    }
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
    /* Your code will be inserted here */
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
