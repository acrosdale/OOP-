#include "../../includes/channels.h"
#include "../../includes/NeighborDiscovery_struct.h"
#include "../../includes/packet.h"



#define BEACON_PERIOD 1000

module NeighborDiscoveryP{
	/// uses interface
	uses interface Timer<TMilli> as beaconTimer;
	uses interface SimpleSend as Sender;
	uses interface Receive;
	provides interface NeighborDiscovery;

	// provides intefaces
	//provides interface
}

implementation{
	neighbor neighborList [MAX_SIZE_neighbor];
	neighbor tempList [MAX_SIZE_neighbor]; // copy of nieghborList_cmp
	neighbor  neighborList_cmp [MAX_SIZE_neighbor];// temp to compare
	uint16_t sentinel = 999;   // known NULL value
	uint16_t counter = 0;		// neighborList global growth counter
	bool check = FALSE;			// use to fill neighborList with sentinel ONCE!!!!
	bool activate = FALSE;		//activate the use of neighborList_cmp to start reviewing changes
	pack MSG;					// instantiation of  packet (message) to send to nodes



	void filler (); // use to fill the neighborList with the known NULL value place holder

	void cleanse ();// use to fill the neighborList_cmp with the known NULL value place holder after use

	void addNeighbor (uint16_t Src); // this fuction add key to the history arr

	void removeNeighbor (uint16_t Src); // this fuction remove key to the Neighborlist

	// use to getNeighborlist from NeighborDiscovery which is PRIVATE
	command neighbor * NeighborDiscovery.getNeighborList(){
		activate = TRUE; //start using nieghborList_cmp to caheck for change
		cleanse (); //fill neighborList_cmp with

		return neighborList;
	}

	command neighbor * NeighborDiscovery.getNeighborListCHANGE(){


		memcpy(tempList, &neighborList_cmp, MAX_SIZE_neighbor * sizeof(neighbor));

		cleanse (); // clean out nieghborList_cmp
		return tempList;
	}

	command void NeighborDiscovery.start(){

		if(check == FALSE){// this f(x) fill the Neighborlist with sentinel ONCE!!!
			filler();
			check= TRUE;
		}

		call beaconTimer.startPeriodic(BEACON_PERIOD);
		//The timer will fire periodically every time period until stopped.

	}

	command void NeighborDiscovery.print(){
		uint16_t i = 0;
		dbg(NEIGHBOR_CHANNEL, "PRINTING NEIGHBOR!\n");

		for(i= 0; i < counter; i++){// go through all "possible" value. VALID if not sentinel
			if(neighborList[i].src != sentinel){

				dbg(NEIGHBOR_CHANNEL, "NEIGHBOR is %d\n", neighborList[i].src);

			}
		}

		
	}

	event void beaconTimer.fired(){ //signals that the timer has expired (one-shot) or repeated (periodic).
		uint16_t i;
		 // the is the source id
		MSG.dest= AM_BROADCAST_ADDR;
		MSG.src= TOS_NODE_ID;

		for (i = 0 ; i < counter ; i++){ // decrease the health of all neighbor by 1 then flood

			if(neighborList[i].health == 0 ){

				removeNeighbor(neighborList[i].src);

			}

			else if(neighborList[i].src != sentinel && neighborList[i].health > 0 ){
				// decrement all of the TTL since last response
				neighborList[i].health -=1;

			}

		}

		call Sender.send(MSG ,AM_BROADCAST_ADDR); // flood all neighbor

	}


	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){
		pack* Msg=(pack*) payload; // typecast the msg to pack type to use
		uint16_t i;


		for (i = 0 ; i < counter; i++){ // check to see if im receiving packet from a neighbor

			if(neighborList[i].health == 0){ // if the health of incoming neighbor is o remove it

				removeNeighbor (neighborList[i].src);

			}

			else if(neighborList[i].src == Msg->src ){ // if true neighbor is responding
				//dbg(NEIGHBOR_CHANNEL, "health increase of %d\n",Msg->src );
				neighborList[i].health = 10;

			}

		}


		if(Msg->dest == AM_BROADCAST_ADDR){// If the destination is AM_BROADCAST, then respond directly.//send(msg, msg.src);

			Msg->dest = Msg->src;
			Msg->src = TOS_NODE_ID;

			call Sender.send(*Msg, Msg->dest);
		}

		else{

			addNeighbor(Msg->src);

		}

		return msg;

	}

		//HELPER FUNCTION. DECLARED ABOVE

		void filler (){ // use to fill the neighborList with the known NULL value place holder
			uint16_t i = 0;

			for (i = 0 ; i < MAX_SIZE_neighbor; i++){// use to fill up neighborList w/place holder value
				neighborList[i].src = sentinel;
				neighborList[i].health = sentinel;

				neighborList_cmp[i].src = sentinel;
				neighborList_cmp[i].health = sentinel;
			}

		}

		void cleanse (){ // use to fill the neighborList_cmp with the known NULL value place holder
			uint16_t i = 0;

			for (i = 0 ; i < MAX_SIZE_neighbor; i++){// use to fill up neighborList_cmp w/place holder value

				neighborList_cmp[i].src = sentinel;
				neighborList_cmp[i].health = sentinel;
			}

		}

		void addNeighbor (uint16_t Src){ // this fuction add key to the history arr
			uint16_t i ,j;
			for (i = 0 ; i < counter ; i++){// check if neighbor exist in the neighborList. PREVENTS ADDING DUPLICATES
				if (neighborList[i].src == Src){// if the neighbor is in the list dont add it.
					return;  // exit() the function
				}
			}
			// if neighbor is not their add it and give it health
			if(activate == TRUE){
				j=0;
				while(j < MAX_SIZE_neighbor){

					if(neighborList_cmp[j].src == sentinel){
						neighborList_cmp[j].src = Src;
						neighborList_cmp[j].health = 10;
					}

					j++;
				}
			}

			if(counter < MAX_SIZE_neighbor){
				neighborList[counter].src = Src;
				neighborList [counter].health= 10;

				counter++; // increase counter signify the amount of "possible" neighbor

				//**** since array is not rearrange counter help combat non arrange array
				// [2, 3, 999 , 4]
				// 2,3 4 are VALID neighbor. 999 is sentinel NULL value

			}
		}

		void removeNeighbor (uint16_t Src){ // this fuction remove key to the Neighborlist
			uint16_t i,j;

			//memcpy(tempList, &neighborList, MAX_SIZE_neighbor * sizeof(neighbor));
			if(activate == TRUE){
				j=0;
				while(j < MAX_SIZE_neighbor){

					if(neighborList_cmp[j].src == sentinel){
						neighborList_cmp[j].src = Src;
						neighborList_cmp[j].health = 16;
					}

					j++;
				}
			}


			for (i = 0 ; i < counter ; i++){// check if neighbor exist in the Neighborlist
				if (neighborList[i].src == Src){// if neighbor is found remove

					neighborList[i].src = sentinel;
					neighborList[i].health = sentinel;

				}
			}

			//*** counter is not decremented as such all value not sentinel ==999
			// are VALID neighbor
		}

}
