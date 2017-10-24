
#ifndef __distance_vector_routing__
#define __distance_vector_routing__
#include "../../includes/NeighborDiscovery_struct.h"


enum{
  ////  MAX_ROUTES =128,           /* MAX size of routing table */
    Router_SIZE =10,
    MAX_COST = 16,         //MAX cost to counter the count‐to‐infinity problem
    len_of_table =32
};


typedef struct {
     uint8_t  Destination;   /* address of destination */
     uint8_t  NextHop;       /* address of next hop */
     uint8_t      Cost;      /* distance metric */
} Router;



typedef nx_struct {
    nx_uint8_t  Dest;   /* address of destination */
    nx_uint8_t      Cost;
    
} Routerpacket;



uint16_t  numRoutes =0;
Routerpacket routingTable[len_of_table];
//32





#endif
