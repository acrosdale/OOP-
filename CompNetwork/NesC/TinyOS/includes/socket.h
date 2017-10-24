

#ifndef _S_H
#define _S_H


# include "protocol.h"
#include "channels.h"

enum{
	
	BUFFER_SIZE = 128,
    SERVER_PORT =88,
    SERVER_ADDR =3,
    WORK_DONE =  1008,
    TO_WRITE  = 12,



};


enum{
    
    
    LISTEN      =0x0B,
    SYN_SEND    =0x0C,
    SYN_RCVD    =0X0D,
    EST         =0X0E,
    
    CLOSING     =0X0F,
    LAST_ACK    =0X10,
    FIN_WAIT    =0X12,
    FIN_WAIT_2  =0X14,
    
    TIME_WAIT   =0X15,
    CLOSED_WAIT =0X18,
    CLOSED      =0X1A,
    
    
    
   

};



typedef struct socket_t{
    
    
    uint8_t  R_buff [BUFFER_SIZE];
    uint8_t  next_expected;
    uint8_t  last_recieve;
    uint8_t  last_read;
    
    uint8_t  S_buff [BUFFER_SIZE];
    uint8_t  last_ack;
    uint8_t  last_send;
    uint8_t  last_write;
    
    uint8_t Src_port;////DO WE NEED THIS
    
    uint8_t Dest_port;
    uint8_t Dest_addr;
    
    uint8_t state;
    bool active;
    
} socket_t;


typedef struct socket_addr_t{

    uint8_t source_Port;/// we have this
    uint8_t source_Addr;
    
}socket_addr_t;


typedef struct JOB{
    
    uint8_t * buff[BUFFER_SIZE];

    uint8_t work;
    uint8_t Transfer;
    bool once;
    

}JOB;







#endif
