#include "ordered_ll.h"
#include "utils.h"
#include "receiver.h"

void testEmptyBuffer1(){
    address_t adda;
    adda.buffer = create_ordered_ll();
    adda.acks = create_ordered_ll();
    adda.last_ack = 0;
    adda.window = 31;
    adda.fd = open("zeb", O_CREAT | O_APPEND | O_WRONLY | O_TRUNC);

    pkt_t* ack1 = ackEncode((uint8_t) 0, 0, 1, adda.window);
    pkt_t* ack2 = ackEncode((uint8_t) 1, 0, 1, adda.window);
    pkt_t* ack3 = ackEncode((uint8_t) 2, 0, 1, adda.window);
    pkt_t* ack4 = ackEncode((uint8_t) 3, 0, 1, adda.window);

    add(adda.buffer, ack1, 0);
    add(adda.buffer, ack2, 0);
    add(adda.buffer, ack3, 0);
    add(adda.buffer, ack4, 0);

    printQ(adda.buffer);

    emptyBuffer(&adda);

    printQ(adda.buffer);

}
