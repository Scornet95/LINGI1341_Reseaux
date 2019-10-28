 #include "packet_interface.h"
#include <stdlib.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <string.h>

/* Extra code */
/* Your code will be inserted here */


pkt_t* pkt_new()
{
    pkt_t* pkt = malloc(sizeof(pkt_t));
    if(pkt == NULL)
        return NULL;
    return pkt;
}

void pkt_del(pkt_t *pkt)
{
    if(pkt != NULL){
        if(pkt->payload != NULL)
            free(pkt->payload);
        free(pkt);
    }
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    int index = 0;

    uint8_t byte = (uint8_t) data[index];
    ptypes_t type = byte >> 6;
    uint8_t tr =  (byte < 1) >> 7;
    uint8_t window = byte & 31;

    if(type != PTYPE_DATA && tr != 1){
        printf("first\n");
        return E_UNCONSISTENT;
    }

    pkt_set_type(pkt, type);
    pkt_set_tr(pkt, tr);
    pkt_set_window(pkt, window);

    index++; // index pointe mtn vers le premier byte de length.

    uint8_t* length = malloc(sizeof(uint8_t) * 2);
    uint16_t* val = malloc(sizeof(uint16_t));
    length[0] = (uint8_t) data[index]; // on récupère le premier byte de length.
    size_t lengthSize = varuint_predict_len(length[0]);

    index++;

    if(lengthSize == 2){
        length[1] = (uint8_t) data[index];
        index++;
    }
    //index se trouve à l'adresse qui suit directement la length.
    varuint_decode(length, lengthSize, val);
    pkt_set_length(pkt, *val);
    free(length);
    free(val);

    if(pkt_get_tr(pkt) == 0){
        if(pkt_get_length(pkt) == 0){
            if(len != (size_t) predict_header_length(pkt)  + (size_t) 4){
                printf("length : %ld, header length : %ld\n", len, (size_t) predict_header_length(pkt));
                return E_UNCONSISTENT;
            }
        }
        else{
            if(len != (size_t) predict_header_length(pkt) + (size_t) pkt_get_length(pkt) + (size_t) 8){
                printf("len : %ld, expected : %ld\n", len, (size_t) predict_header_length(pkt) + (size_t) pkt_get_length(pkt) + (size_t) 8);
		        printf("ici\n");
                return E_UNCONSISTENT;
            }
        }
    }
    else{
        printf("tr à 1 fdp\n");
        if(len != (size_t) predict_header_length(pkt) + (size_t) 4)
            return E_UNCONSISTENT;
    }

    uint8_t seqnum = (uint8_t) data[index];
    pkt_set_seqnum(pkt, seqnum);

    index++;//index se trouve à l'adresse de timestamp

    uint32_t timestamp = 0;

    timestamp += (uint32_t) data[index] & 0x000000ff;
    index++;
    uint32_t second = (((uint32_t) data[index]) << 8) & 0x0000ff00;
    timestamp += second;
    index++;
    uint32_t third = (((uint32_t) data[index]) << 16) & 0x00ff0000;
    timestamp += third;
    index++;
    uint32_t fourth = (((uint32_t) data[index]) << 24) & 0xff000000;
    timestamp += fourth;
    index++;

    pkt_set_timestamp(pkt, timestamp);


    uint32_t crc1 = 0;

    crc1 += ((uint32_t) ((uint8_t) data[index])) << 24;
    index++;
    crc1 += ((uint32_t) ((uint8_t) data[index])) << 16;
    index++;
    crc1 += ((uint32_t) ((uint8_t) data[index])) << 8;
    index++;
    crc1 += (uint32_t) ((uint8_t) data[index]);
    index++;

    pkt_set_crc1(pkt, crc1);

    char* buf = malloc(predict_header_length(pkt));
    memcpy(buf, data, predict_header_length(pkt));
    buf[0] = buf[0] & 223;
    crc1 = crc32( 0L, Z_NULL, 0);
    crc1 = crc32(crc1, (const unsigned char *) buf, predict_header_length(pkt));
    free(buf);

    if(crc1 != pkt_get_crc1(pkt)){
        return E_CRC;
    }

    if(pkt_get_type(pkt) != PTYPE_DATA || pkt_get_tr(pkt) == 1 || pkt_get_length(pkt) == 0)
        return PKT_OK;

    char* payload = malloc(sizeof(char) * pkt_get_length(pkt));

    int i;
    for(i = 0; i < pkt_get_length(pkt); i++, index++){
        payload[i] = data[index];
    }

    pkt_set_payload(pkt, payload, pkt_get_length(pkt));


    uint32_t crc2 = 0;
    crc2 += ((uint32_t) ((uint8_t) data[index])) << 24;
    index++;
    crc2 += ((uint32_t) ((uint8_t) data[index])) << 16;
    index++;
    crc2 += ((uint32_t) ((uint8_t) data[index])) << 8;
    index++;
    crc2 += (uint32_t) ((uint8_t) data[index]);

    pkt_set_crc2(pkt, crc2);

    crc2 = crc32( 0L, Z_NULL, 0);
    crc2 = crc32(crc2, (const unsigned char*) payload, pkt_get_length(pkt));
    if(crc2 != pkt_get_crc2(pkt)){
        free(payload);
        return E_CRC;
    }
    free(payload);
    return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    int index = 0;

    //On crée un premier byte contenant les 3 premiers champs.
    uint8_t byte = 0;

    ptypes_t type = pkt_get_type(pkt);
    uint8_t tr = (uint8_t) 0;
    uint8_t window = pkt_get_window(pkt);

    byte = ((uint8_t) type) << 6;
    byte += tr << 5;
    byte += window;
    buf[index] = (char) byte;

    index++; //L'index est à 1, l'adresse pointe vers le premier byte de length.

    uint16_t pktLength = pkt_get_length(pkt);
    ssize_t l = varuint_predict_len(pktLength);
    uint8_t* length = malloc(sizeof(uint8_t) * l);
    varuint_encode(pktLength, length, l);
    buf[index] = (char) length[0];

    index++;

    if(l == 2){
        buf[index] = (char) length[1];
        index++;
    }// index = 2 ou 3 en fonction de la taille du varuint. Il se trouve à l'adresse où ira seqnum.
    free(length);

    uint8_t seqnum = pkt_get_seqnum(pkt);
    buf[index] = (char) seqnum;

    index++; //Index est à l'adresse où sera le timestamp.

    uint32_t timestamp = pkt_get_timestamp(pkt);
    uint8_t* time = malloc(sizeof(uint8_t) * 4);
    time[0] = (timestamp & 0x000000ff);
    time[1] = (timestamp & 0x0000ff00) >> 8;
    time[2] = (timestamp & 0x00ff0000) >> 16;
    time[3] = (timestamp & 0xff000000) >> 24;

    int i;
    for(i = 0; i < 4; i++, index++){
        buf[index] = (char) time[i];
    }
    free(time);
    //index est à l'adresse où sera le crc1;

    uint32_t crc1 = crc32(0L, Z_NULL, 0);
    crc1 = crc32(crc1, (const unsigned char *) buf, predict_header_length(pkt));
    pkt_set_crc1((pkt_t*) pkt, (const uint32_t) crc1);

    uint8_t* crc = malloc(sizeof(uint8_t) * 4);
    crc[0] = (crc1 & 0x000000ff);
    crc[1] = (crc1 & 0x0000ff00) >> 8;
    crc[2] = (crc1 & 0x00ff0000) >> 16;
    crc[3] = (crc1 & 0xff000000) >> 24;

    for(i = 0; i < 4; i++, index++){
        buf[index] = (char) crc[3 - i];
    }

    free(crc);

    if(type == PTYPE_DATA){
        const char* payload = pkt_get_payload(pkt);
        for(i = 0; i < pkt_get_length(pkt); i++, index++){
            buf[index] = payload[i];
        }
    }
    else{
        return PKT_OK;
    }

    //index se situe mtn après le payload, nous devons encoder le crc2.
    uint32_t crc2 = crc32(0L, Z_NULL, 0);
    crc2 = crc32(crc2, (const unsigned char *) pkt_get_payload(pkt), pkt_get_length(pkt));
    pkt_set_crc2((pkt_t*) pkt, (const uint32_t) crc2);

    crc[0] = (crc2 & 0x000000ff);
    crc[1] = (crc2 & 0x0000ff00) >> 8;
    crc[2] = (crc2 & 0x00ff0000) >> 16;
    crc[3] = (crc2 & 0xff000000) >> 24;

    for(i = 0; i < 4; i++, index++){
        buf[index] = (char) crc[3 - i];
    }

    free(crc);

    *len = index;
    return PKT_OK;
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt){
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt){
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt){
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt){
    return pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt){
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt){
    return pkt->crc1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt){
    return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt){
    return pkt->payload;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type){

    if(type != 1){
        if(type != 2){
            if(type != 3){
                return E_TYPE;
            }
        }
    }

    pkt->type = type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr){

    if(tr != 0 && tr != 1){
        return E_TR;
    }
    pkt->tr = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window){

    pkt->window = window;
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum){

    pkt->seqnum = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length){
    if(length > 512) return E_LENGTH;
    pkt->length = length;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp){
    pkt->timestamp = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1){
    pkt->crc1 = crc1;
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2){
    pkt->crc2 = crc2;
    return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,const char *data,const uint16_t length){

    pkt_set_length(pkt, length);
    char * payload = (char *) malloc(length);
    if(payload == NULL){
        printf("ici c'est sur\n");
        return E_UNCONSISTENT;
    } 

    memcpy(payload,data,length);
    pkt->payload = payload;
    return PKT_OK;

}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval){
    if(len!= 1 && len!=2){
        return -1;
    }

    if(varuint_len(data)==1){
        *retval=(uint16_t)data[0];
        return 1;
    }
    else if(varuint_len(data)==2){
		uint16_t val;
		val = (((uint16_t) data[0]) & 127)<<8;
		val += (uint16_t) data[1];
		*retval = val;
		return 2;
    }
    else{
        // le varuint n est pas valide
        return -1;
    }

    return -1;
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len){
    if((ssize_t) len < varuint_predict_len(val)) return -1;
    if(varuint_predict_len(val) == 1){
        data[0] = (uint8_t) val;
        return 1;
    }
    else{
        uint8_t firstByte = (uint8_t) (val >> 8);
        data[0] = firstByte | 128;
        uint8_t secondByte =(uint8_t) (val);
        data[1] = secondByte;
        return 2;
    }

}

size_t varuint_len(const uint8_t *data)
{
    if((data[0]  & 128) == 0) return 1;
    else return 2;
}


ssize_t varuint_predict_len(uint16_t val)
{
    if(val > 0x8000) return -1;
    else if(val > 127) return 2;
    else return 1;


}


ssize_t predict_header_length(const pkt_t *pkt)
{
    uint16_t length = pkt_get_length(pkt);
    int val =  varuint_predict_len(length);
    return val + 6;
}
