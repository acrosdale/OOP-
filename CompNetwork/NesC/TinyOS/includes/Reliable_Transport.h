
#ifndef __Reliable_Transport__
#define  __Reliable_Transport__
#include "../../includes/packet.h"
#include "../../includes/socket.h"



enum{
    len_of_buff =32,
    TRANSPORT_SIZE= 10,
    HEADER_LENGTH = 8,
    MAX_PAYLOAD = PACKET_MAX_PAYLOAD_SIZE  - HEADER_LENGTH,
    
    //flags

    FIN = 0x1,//close connection
    SYN = 0x02,//used during connection establishment
    SYN_ACK = 0x04,
    ACK = 0x06,
    DAT = 0x08,
    DAT_ACK = 0x0A,
    
    
    
};



typedef nx_struct Transfer {// Transport struct
    
    nx_uint8_t   SrcPort;
    nx_uint8_t   DstPort;
    nx_uint8_t  SeqNum;
    nx_uint8_t  Acknowledge;
    nx_uint8_t   AdvertisedWindow; //flow control
    nx_uint8_t   Flag;
    nx_uint8_t   length;
    
    
    nx_uint8_t DATA[MAX_PAYLOAD];
    //12bytes
    

}Transfer;




typedef struct transmission {
    
    uint16_t type;
    uint16_t dest;
    uint16_t time;
    uint16_t SeqNum;
    socket_t * fd;

    
    
} transmission;



typedef struct conn {
    
    uint8_t  read_buff[BUFFER_SIZE];
    uint8_t  APP_buff[BUFFER_SIZE];
    socket_t * fd;
    char  name [20];
    uint8_t start;
    uint8_t  finish;
    
} conn;





Transfer Transit[len_of_buff];


#endif
