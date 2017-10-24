#include "../../includes/channels.h"
#include "../../includes/Flooding_struct.h"
#include "../../includes/packet.h"

module FloodingP{
	provides interface SimpleSend as FloodSender;
	provides interface Receive as MainReceive;
	provides interface Receive as ReplyReceive;

	// Internal
	uses interface SimpleSend as InternalSender;
	uses interface Receive as InternalReceiver;
}

implementation{
	track history[MAX_SIZE_history]; //** keep track of src and seq
	uint16_t counter = 0; // global counter to represent how filled is the history
	uint16_t counter_infinite= 0;
	uint16_t sentinel = 999; // Known NULL VAlue place holder
	uint16_t seq= 0;

	bool check = FALSE;// used to check if arr isEmpty()

	void filler (); // use to fill the history with the place holder

	void addKeys_TO_histoy (uint16_t Src,  uint16_t Seq); // this fuction add key to the history arr

	bool checkKeys_IN_history (uint16_t Src,  uint16_t Seq); // this function check the key in the history

	void ping (pack msg, uint16_t dest, uint8_t protocol){

		if (check == FALSE){
			filler();
			check = TRUE;
		}

		msg.TTL = MAX_TTL; // max time to live in the Network
		msg.src = TOS_NODE_ID; // source nodes. starter nodes
		msg.dest = dest;
		msg.protocol = protocol;  // ping protocol
		msg.seq = seq++;	// seq # represent the packet # in the network
		addKeys_TO_histoy(msg.src, msg.seq); // add the source nodes. no check needed, it is the first one.
		dbg(FLOODING_CHANNEL, "Flooding Network: %s\n", msg.payload);
		call InternalSender.send(msg, AM_BROADCAST_ADDR); //Flood Neighbor/ network

	}

	command error_t FloodSender.send(pack msg, uint16_t dest){

		// this function call filler f(x) above to fill the arr w/ sentinel

		ping(msg, dest, 0);

		return SUCCESS;
	}

  	event message_t* InternalReceiver.receive(message_t* msg, void* payload, uint8_t len){

		pack* Msg = (pack*) payload;

		//dbg(FLOODING_CHANNEL, "Receive: %s\n", Msg->payload);

		if (checkKeys_IN_history(Msg->src, Msg->seq)== TRUE){ // Check to see if we have seen it before?
			// If we have return msg;
			//dbg(FLOODING_CHANNEL, "MESSAGE SEEN BEFORE: DROPPED!!\n");
			return msg;
		}

		if  (checkKeys_IN_history(Msg->src, Msg->seq)== FALSE){ // Check to see if we have seen it before?
			// If we haven’t
			addKeys_TO_histoy(Msg->src, Msg->seq); // add it to keys/ history

			if(Msg->dest == TOS_NODE_ID){
				// RESPOND if it is not a ping reply && msg.Protocol != PING_REPLY


			 	if(Msg->protocol != 1){
					check = FALSE;
					ping(*Msg, Msg->src, 1);
					return signal MainReceive.receive(msg, payload, len);
				}
				else{
					return signal ReplyReceive.receive(msg, payload, len);
				}

			}

		}

		// If TTL Expired return msg;
		if(Msg->TTL == 0){
			return msg;
		}

		if(Msg->dest != TOS_NODE_ID){// if not final destination
			Msg->TTL -= 1;// decrement TTL
			call InternalSender.send(*Msg , AM_BROADCAST_ADDR); //call FloodSender.send(*Msg , AM_BROADCAST_ADDR);
			logPack(Msg);
		}

		return msg;
   	}


	void filler (){ // use to fill the history with the place holder
		uint16_t i = 0;

		for (i = 0 ; i < MAX_SIZE_history; i++){// use to fill up history place holder value
			history[i].seq = sentinel;
			history[i].src = sentinel;
		}

	}

	void addKeys_TO_histoy (uint16_t Src,  uint16_t Seq){ // this fuction add key to the history arr
		uint16_t i = 0;

		for (i = 0 ; i < counter ; i++){ //PREVENTS ADDING DUPLICATES

			if (history[i].src == Src && history[i].seq == Seq){
				return; // exit() if dupplicates found
			}
		}


		history[counter_infinite % MAX_SIZE_history].src = Src;
		history[counter_infinite % MAX_SIZE_history].seq = Seq;

		if(counter < MAX_SIZE_history){
			counter++; // keep count of "possible" neighbor

		}
		counter_infinite++;
	}

	bool checkKeys_IN_history (uint16_t Src,  uint16_t Seq){ // this function check the key in the history
		uint16_t i = 0;

		for (i = 0 ; i < counter; i++){ // key is found retun TRUE

			if(history[i].seq == Seq && history[i].src == Src){
				return TRUE;
			}
		}
		return FALSE; // key was not found
	}

}
