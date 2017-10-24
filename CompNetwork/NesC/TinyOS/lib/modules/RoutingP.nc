#include "../../includes/channels.h"
#include "../../includes/NeighborDiscovery_struct.h"
#include "../../includes/packet.h"
#include "../../includes/distance_vector_routing.h"
#include<stdio.h>
#include<stdlib.h>

#define BEACON 1000
#define T_size 255
#define ONESHOT_PERIOD 100000

module RoutingP{
	/// uses interface
	uses interface Timer<TMilli> as beaconTimer;
	uses interface Timer<TMilli> as oneShot;

	//send special packet over network
	uses interface SimpleSend    as packetSending;
	uses interface Receive       as packetReceived;

	//send msg over network
	uses interface SimpleSend    as InternalSending;
	uses interface Receive       as InternalReceived;

	//use to send ping and reply over network
	provides interface SimpleSend as RouteSender;
	provides interface Receive as MainReceive;
	provides interface Receive as ReplyReceive;


	uses interface NeighborDiscovery;

	uses interface Transport as Transporter;

	// provides inteface
	provides interface Routing;


}

implementation{
	//int16_t numRoutes = 0; //**** DECLARED IN THE STRUCT FILE
    //Router routingTable [MAX_ROUTES]; //**** DECLARED IN THE STRUCT FILE
	neighbor * Route_NeighborList;
	Router miniRTable[T_size]; //local table
	Router spotControl [len_of_table];// use to save spot of value that turn off, incase they ever return
	Routerpacket routepack [len_of_table];
	pack MSG;
	uint8_t SENTINEL = 99;
	uint16_t infinity = 999;
	uint16_t numRoutes;		// neighborList global growth counter
	uint16_t SEQ = 0; // seq of Package
	bool checker = FALSE;
	bool oneshot_done = FALSE;

	int nieghbor_TSIZE ();
	bool isCHANGE();
	void filler();
	void Routelog (pack * input);
	void payload_send();
	void save_spot( uint16_t DEST, uint16_t NEXT, uint16_t COST);
	void getSpot(uint16_t spot);
	void find_dest(pack msg, uint16_t DESTINATION);

	void updateRoutingTable (Routerpacket * newRoute, uint16_t numNewRoutes, uint16_t source);
		/*this routine that calls mergeRoute to incorporate all the routes contained in
   	   a routing update that is received from a neighboring node.*/

	void mergeRoute (Routerpacket * newRoutes, uint16_t NH);
   	//The routine that updates the local node’s routing table based on a new route

	command Router * Routing.getRTable(){

		return miniRTable;

	}

	command void Routing.start(){

		if(checker== FALSE){

			filler();

			checker= TRUE;
		}


		call oneShot.startOneShot(ONESHOT_PERIOD);


		call beaconTimer.startPeriodicAt(ONESHOT_PERIOD,BEACON);


	}



	void ping (pack msg, uint16_t dest, uint8_t protocol){

		msg.TTL = MAX_TTL; // max time to live in the Network
		msg.src = TOS_NODE_ID; // source nodes. starter nodes
		msg.dest = dest;
		msg.protocol = protocol;  // ping protocol
		msg.seq = SEQ++;	// seq # represent the packet # in the network
		find_dest(msg, dest);

	}
	command void Routing.trans (pack msg, uint16_t dest, uint8_t protocol){
		//dbg(TRANSPORT_CHANNEL, "R_ Transporting... \n");
		msg.TTL = MAX_TTL; // max time to live in the Network
		msg.src = TOS_NODE_ID; // source nodes. starter nodes
		msg.dest = dest;
		msg.protocol = protocol;  // ping protocol
		msg.seq = SEQ++;	// seq # represent the packet # in the network
		find_dest(msg, dest);

	}


	command error_t RouteSender.send(pack msg, uint16_t dest){

		ping(msg, dest, 0);

		return SUCCESS;
	}


	command void Routing.print(){

		uint16_t i, keyD,keyN,keyC, j;

   		for (i = 1; i < T_size; i++){

			if(miniRTable[i].Destination == SENTINEL){
				continue;
			}

       		keyD = miniRTable[i].Destination;
			keyN = miniRTable[i].NextHop;
			keyC = miniRTable[i].Cost;
       		j = i-1;

       		while (j > 0 && miniRTable[j].Destination > keyD){

           		miniRTable[j+1] = miniRTable[j];
           		j--;
       		}

	   		miniRTable[j+1].Destination = keyD;
			miniRTable[j+1].NextHop = keyN;
			miniRTable[j+1].Cost = keyC;
   		}


		dbg(NEIGHBOR_CHANNEL, "PRINTING routing !\n");
		dbg(NEIGHBOR_CHANNEL, "Dest     \t\t\tNextHop          \t\t\tCost\n");

		for(i= 0; i < T_size; i++){// go through all "possible" value. VALID if not sentinel

			if(miniRTable[i].Destination != SENTINEL){

				dbg(ROUTING_CHANNEL,"%d\t",  miniRTable[i].Destination);
				dbg(ROUTING_CHANNEL,"%d\t", miniRTable[i].NextHop);
				dbg(ROUTING_CHANNEL,"%d \n", miniRTable[i].Cost);

			}

		}



	}

	event void oneShot.fired(){
		//***1
		//Initially, each node sets a cost of 1 to its directly connected neighbors and ∞ to all other nodes

		uint16_t i;
		Route_NeighborList = call NeighborDiscovery.getNeighborList();

		for(i = 0; i < nieghbor_TSIZE (); i++){


			if(Route_NeighborList[i].src != infinity){


				miniRTable[i].Destination  = Route_NeighborList[i].src;
				miniRTable[i].NextHop      = Route_NeighborList[i].src;
				miniRTable[i].Cost         = 1;
				numRoutes++;
			}

		}
		oneshot_done = TRUE;
		//call beaconTimer.startPeriodic(BEACON);
	}

	event void beaconTimer.fired(){ //signals that the timer has expired (one-shot) or repeated (periodic).
		uint16_t i,j, badNext;
		MSG.src= TOS_NODE_ID;

		//2 The next step  is that every node sends a message to its
		//directly connected neighbors containing its personal list of distance
		// if neighbors disconeect make note

		if(isCHANGE()==TRUE){
			//dbg(ROUTING_CHANNEL,"Table CHANGE!! :O. recalibrating RTABLE... =^_^=  \n");
		}

		i =0;
		j= 0;
		badNext=0;
		while ( j < T_size) // check to make sure disconected Node are NOT relied on
		{
			if(miniRTable[j].Cost == MAX_COST){
				badNext = miniRTable[j].Destination;
			}

			if(badNext != 0){

				while ( i < T_size){

					if(badNext == miniRTable[i].NextHop){
						miniRTable[i].Cost= MAX_COST;
					}

					i++;
				}

			}
			j++;
		}


		i =0;
		j= 0;
		//		0-10               0-254
		while (i < Router_SIZE && j < T_size)
		{

			if(miniRTable[j].Destination != SENTINEL){

				routepack[i].Dest = miniRTable[j].Destination;
				routepack[i].Cost = miniRTable[j].Cost;

				i++;
			}

			j++;

			if(i == Router_SIZE){

				memcpy(MSG.payload, &routepack, Router_SIZE * sizeof(Routerpacket));

				payload_send();

				i=0;
			}
		}

		memcpy(MSG.payload, &routepack, Router_SIZE * sizeof(Routerpacket));
		payload_send();


	}


	event message_t* InternalReceived.receive(message_t* msg, void* payload, uint8_t len){
		pack* Msg = (pack*) payload;

		///Routelog(Msg);

		if(Msg->dest == TOS_NODE_ID){
			// RESPOND if it is not a ping reply && msg.Protocol != PING_REPLY

			if(Msg->protocol == PROTOCOL_TCP){
				//dbg(ROUTING_CHANNEL,"PROTOCOL_TCP detected\n");
				call Transporter.receive(Msg);
				//return signal MainReceive.receive(msg, payload, len);
			}


			if(Msg->protocol == PROTOCOL_PING ){

				ping(*Msg, Msg->src, PROTOCOL_PINGREPLY);
				return signal MainReceive.receive(msg, payload, len);
			}

			if (Msg->protocol == PROTOCOL_PINGREPLY){

				return signal ReplyReceive.receive(msg, payload, len);
			}

		}

		if(Msg->TTL == 0){
			return msg;
		}

		if(Msg->dest != TOS_NODE_ID){// if not final destination
			Msg->TTL -= 1;// decrement TTL

			find_dest(*Msg, Msg->dest);

		}

		return msg;
	}


	event message_t* packetReceived.receive(message_t* msg, void* payload, uint8_t len){

		uint16_t j=0;
		uint16_t num_Routes;
		pack* Msg=(pack*) payload;
		pack * RT = (pack*) payload; //local nieghbor

		Routerpacket * RTable = (Routerpacket*) &RT->payload;

		j=0;
		while (j < Router_SIZE){

			if(RTable[j].Dest != SENTINEL){
				num_Routes++;
			}

			j++;
		}

		updateRoutingTable(RTable, num_Routes, Msg->src);

		return msg;
	}


	//this routine that calls mergeRoute to incorporate all the routes contained in
	//a routing update that is received from a neighboring node.

	void updateRoutingTable (Routerpacket * newRoute, uint16_t numNewRoutes, uint16_t source ){
		uint16_t i;
		//bg(ROUTING_CHANNEL,"Receive BEACON %d\n", newRoute[4].Dest);

		uint16_t nextH = 0; // store the position of the next-hop
		nextH = source;


      	for (i=0; i < numNewRoutes ; i++){

			mergeRoute(&newRoute[i], nextH);

      	}
	}

//The routine that updates the local node’s routing table based on a new route
	void mergeRoute (Routerpacket * newRoutes, uint16_t NH){
		uint16_t i;
		bool ONLY_ONCE = FALSE;

		for (i = 0; i < numRoutes ; ++i) {

			if(newRoutes->Dest == TOS_NODE_ID){
				return;
			}

        	if (newRoutes->Dest == miniRTable[i].Destination) {


				if(NH == miniRTable[i].NextHop && newRoutes->Cost <= MAX_COST){//count to infinity

					miniRTable[i].Cost = ++newRoutes->Cost;

					if(newRoutes->Cost > MAX_COST){
						newRoutes->Cost = MAX_COST;
						miniRTable[i].Cost = newRoutes->Cost;
					}

					return;
				}


            	if(newRoutes->Cost + 1 < miniRTable[i].Cost){ //split horizon w/poisen
                	/* found a better route: */
                	break;
            	}

				else {
                	/* route is uninteresting---just ignore it */
                	return;
            	}
        	}

			if ( (i+1) == numRoutes && ONLY_ONCE == FALSE) {
				//dbg(ROUTING_CHANNEL,"ONCE MORE\n");

				ONLY_ONCE = TRUE;

				if (numRoutes < T_size) {
					++numRoutes;
				}
				else {
	              	/* can't fit this route in table so give up */
				  	return;
			   	}

    		}

		}

    	miniRTable[i].Destination = newRoutes->Dest;
		miniRTable[i].NextHop = NH;
		miniRTable[i].Cost = ++newRoutes->Cost;

	}

	void payload_send(){

		uint16_t z = 0;
		uint16_t i;

		Router SendRTable[T_size];

		memcpy(SendRTable, &miniRTable, T_size * sizeof(Router));

		z = 0;
		while( z < T_size)
		{

			if(SendRTable[z].Destination != SENTINEL && SendRTable[z].Cost == 1){
				//POISON WHEN SENDING BACK TO NEIGHBOR

				//SPLIT HORIZON W/ POISON REVERSE
				i = 0;
				while( i < T_size)// POISEN ONLY THE ONES FROM A Neighbor
				{
					if(SendRTable[i].NextHop == SendRTable[z].Destination){

						SendRTable[i].Cost = MAX_COST;
					}
					i++;

				}

				MSG.dest = SendRTable[z].Destination;
				call packetSending.send(MSG , MSG.dest);
			}

			z++;
		}

	}

	void find_dest(pack msg, uint16_t DESTINATION){

		uint16_t i = 0;

		while(i < T_size){

			if(miniRTable[i].Destination == DESTINATION && miniRTable[i].Cost != MAX_COST){
				//dbg(ROUTING_CHANNEL, "NextHop %d\n", miniRTable[i].NextHop);

				call InternalSending.send(msg, miniRTable[i].NextHop);
			}

			if(miniRTable[i].Destination == DESTINATION && miniRTable[i].Cost == MAX_COST){
				dbg(ROUTING_CHANNEL,"NODE IS UNREACHABLE\n");
					// \u2620
			}

			i++;
		}

	}
	void save_spot( uint16_t DEST, uint16_t NEXT, uint16_t COST){
		uint16_t i =0;

		for(i= 0; i < len_of_table; i++){//check to see if value exist

			if(DEST == spotControl[i].Destination){
				return;
			}

		}

		for(i= 0; i < len_of_table; i++){//check to see if value exist

			if(spotControl[i].Destination == SENTINEL){

				spotControl[i].Destination = DEST;
				spotControl[i].NextHop = NEXT;
				spotControl[i].Cost = COST;

				return;
			}

		}


	}

	void getSpot(uint16_t spot){
		uint16_t i,j;

		for(i= 0; i < len_of_table; i++){//check to see if value exist

			if(spot == spotControl[i].Destination){

				for(j= 0; i < T_size; j++){

					if(spot == miniRTable[j].Destination){

						miniRTable[j].Destination = spotControl[i].Destination;
						miniRTable[j].NextHop = spotControl[i].NextHop;
						miniRTable[j].Cost = spotControl[i].Cost;

						spotControl[i].Destination = SENTINEL;
						spotControl[i].NextHop = SENTINEL;
						spotControl[i].Cost = MAX_COST;
						return;
					}

				}

			}

		}

	}


	bool isCHANGE(){

		uint16_t i,j;

		bool isChange = FALSE;

		neighbor * Check_Route  = call NeighborDiscovery.getNeighborListCHANGE();

		for(i= 0; i < MAX_SIZE_neighbor; i++){//check to see if value exist. which signify change
			if(Check_Route[i].src == infinity && i == MAX_SIZE_neighbor-1){

				//dbg(ROUTING_CHANNEL,"CHANGESSSS%d\n",Check_Route[i].src);
				return isChange;
			}

		}

		isChange = TRUE;

		for(i= 0; i < MAX_SIZE_neighbor; i++){//find disconected values still in the RTABLE and update value cost

			if(Check_Route[i].src != infinity){

				for(j= 0; j < T_size; j++){

					if(miniRTable[j].Destination == SENTINEL){
						continue;
					}

					else if(Check_Route[i].src == miniRTable[j].Destination && Check_Route[i].health == MAX_COST){

						save_spot(miniRTable[j].Destination, miniRTable[j].NextHop, miniRTable[j].Cost );
						miniRTable[j].Cost    = MAX_COST;

						break;
					}

					else if(Check_Route[i].src == miniRTable[j].Destination && Check_Route[i].health <=10){

						getSpot(miniRTable[j].Destination);

						break;
					}
				}

			}

		}

		return isChange;
	}


	int nieghbor_TSIZE (){
		uint16_t i = 0;

		while(Route_NeighborList[i].src != infinity){

			i++;
		}

		return i;
	}

	void Routelog (pack * input){
		uint16_t i = 0;

		while(i < T_size){

			if(miniRTable[i].Destination == input->dest){

				dbg(ROUTING_CHANNEL, "Routing Packet- Src: %hhu Dest: %hhu Seq: %hhu NextHop: %hhu Cost:%hhu \n",
				input->src, input->dest, input->seq, miniRTable[i].NextHop, miniRTable[i].Cost);
				break;
			}
			i++;
		}

	}

	void filler (){ // use to fill the neighborList with the known NULL value place holder
		uint16_t i = 0;

		for (i = 0 ; i < T_size; i++){// use to fill up neighborList w/place holder value

			miniRTable[i].Destination = SENTINEL;
			miniRTable[i].NextHop = SENTINEL;
			miniRTable[i].Cost = MAX_COST;

			if(i < len_of_table){
				spotControl[i].Destination = SENTINEL;
				spotControl[i].NextHop = SENTINEL;
				spotControl[i].Cost = MAX_COST;

				routepack[i].Dest = SENTINEL;
				routepack[i].Cost= MAX_COST;

			}

		}

	}


	event void * Transporter.connectDone( socket_t * FD){}

	event void * Transporter.accept( socket_t * FD){}


}
