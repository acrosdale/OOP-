#include "../../includes/channels.h"
#include "../../includes/NeighborDiscovery_struct.h"
#include "../../includes/packet.h"
#include "../../includes/distance_vector_routing.h"
#include "../../includes/Reliable_Transport.h"
#include "../../includes/packet.h"

//#include <sys/socket.h>

#include<stdio.h>
#include<stdlib.h>

#define TIME_WAIT 60000
#define T_size 255


#define RTT 9000
//1000 ms == 1sec

module TransportP{
	/// uses interface
	uses interface Timer<TMilli> as WaitTimer;
	uses interface Timer<TMilli> as oneShot;

	//send special packet over network
	//uses interface SimpleSend    as packetSending;
	//uses interface Receive       as packetReceived;

	//send msg over network
	//uses interface SimpleSend    as InternalSending;
	//uses interface Receive       as InternalReceived;

	//use to send ping and reply over network
	provides interface SimpleSend as transSender;
	provides interface Receive as MainReceive;
	provides interface Receive as ReplyReceive;

	uses interface Routing;

	// provides inteface
	provides interface Transport;


	uses interface Pool<socket_t>;

	//uses interface Pool<socket_t> as Pool_addr;

	uses interface Queue<transmission>;

	uses interface Hashmap<socket_t*> as Hash;

}

implementation{



	Transfer* TCP;
	Router * RTable;;

	uint8_t t_count = 0;
	pack MSG;
	uint16_t SEQ = 0; // seq of Package
	uint8_t SENTINEL = 999;
	bool once = FALSE;

	uint8_t port_count = 0;
	uint8_t ports[255];

	bool DONE = FALSE;
	bool CLIENT_SIDE= FALSE;
	bool SERVER_SIDE= FALSE;

	uint8_t timeout =0;

	uint8_t tracker =0;

	uint8_t WORK_LEFT =WORK_DONE;

	void filler ();
	uint8_t getPort();
	void Free_port (uint8_t port);
	void Socketlog(socket_t * input);
	uint8_t min(uint8_t EF,uint8_t PAYLOAD,uint8_t data_it );


	void retranmission(uint8_t Flag, uint16_t dest,uint16_t SeqNum,pack msg, socket_t *fd){


		uint16_t dt,sent;
		transmission transmit;
		transmit.type   = Flag;
		transmit.dest   = dest;
		transmit.SeqNum = SeqNum;

		sent = call oneShot.getNow();
		transmit.time =  sent + RTT;
		transmit.fd   = fd;
		transmit.drop = 1;
		call Queue.enqueue(transmit);
		//dbg(TRANSPORT_CHANNEL, "sending to  %hhu \n", dest);

		call transSender.send(msg, dest);
		dt = call oneShot.getNow() + (sent + (2* RTT));

		if(call oneShot.isRunning() == FALSE){

			call oneShot.startOneShot(dt);

		}

	}

	command error_t transSender.send(pack msg, uint16_t dest){

		call Routing.trans(msg, dest, PROTOCOL_TCP);

		return SUCCESS;
	}

	command void Transport.print (uint8_t *buff, uint16_t bufflen){
		//R_buff [BUFFER_SIZE]
		uint16_t i=0;
		for(i = 0; i < bufflen; i++){


			dbg(TRANSPORT_CHANNEL, "Reading Data: %hhu %hhu %hhu %hhu %hhu\n",buff[i],buff[i+1],buff[i+2],buff[i+3],buff[i+4]);
			i+=5;


		}

		//tracker = buff[bufflen];


	}


	command socket_t * Transport.socket(){

		socket_t * NEW = NULL;

		if(!call Pool.empty()){

        	NEW = call Pool.get();
           	return NEW;
        }

        return NEW;
	}


    command error_t Transport.bind(socket_t* fd, socket_addr_t * addr){
		//binds the port to it address
		//addrs->source_Port = SERVER_PORT; //123

		fd->Src_port = addr->source_Port;

        return SUCCESS;

	}



	command uint16_t Transport.read(socket_t *fd, uint8_t *buff, uint16_t bufflen){
		//Read from the socket and write this data to the buffer. This data
	    // is obtained from your TCP implimentation.
		uint8_t len=0,i;

		//dbg(TRANSPORT_CHANNEL, "last_read: %hhu last_recieve %hhu  \n",fd->last_read,fd->last_recieve);

		if(fd->state == EST){

			if(fd->last_recieve > fd->last_read){ //normal case

				if(bufflen <  BUFFER_SIZE -(fd->last_recieve - fd->last_read)  ){ //TAKE ALL OF IT
				//dbg(TRANSPORT_CHANNEL, "DAT");
					len = bufflen;
				}
				else{
				//dbg(TRANSPORT_CHANNEL, "DAT2");
					len = BUFFER_SIZE - (fd->last_recieve - fd->last_read);

				}
			}

			if(fd->last_read > fd->last_recieve ){ //wrap around

				if(bufflen <  BUFFER_SIZE - (fd->last_read - fd->last_recieve)  ){ //TAKE ALL OF IT

					len = bufflen;
				}
				else{

					len = BUFFER_SIZE  - (fd->last_read - fd->last_recieve);

				}
			}





			for(i = 0; i < len ; i++){


				buff[i]= fd->R_buff[(fd->last_read)% BUFFER_SIZE];

				//dbg(TRANSPORT_CHANNEL, "DATA BUFF: %hhu\n",buff[i] );
				++fd->last_read;

			}
		}

        return len;
    }



    command uint16_t Transport.write(socket_t* fd, uint8_t *buff, uint16_t bufflen){
		//write to the send buffer
		uint8_t len,i,to_write, data_left, EFF;
		TCP = (Transfer*) MSG.payload;

		if(fd->state == EST){



				if(fd->last_write > fd->last_ack){

					if(bufflen < (BUFFER_SIZE -(fd->last_write - fd->last_ack) ) ){ //TAKE ALL OF IT
						len = bufflen;
					}

					else { // TAKE A PORTION
						len = ((fd->last_write - fd->last_ack)); //take all the remaining buffer
					}

				}

				else{

					if(bufflen < BUFFER_SIZE -(fd->last_ack - fd->last_write)  ){ //TAKE ALL OF IT
						len = bufflen;
					}

					else{ // TAKE A PORTION
						len = BUFFER_SIZE -(fd->last_ack - fd->last_write); //take all the remaining buffer
					}

				}



			//dbg(TRANSPORT_CHANNEL, "DATA Clen: %hhu\n",len);
			//							data in flight
			EFF = TCP->AdvertisedWindow - (BUFFER_SIZE - len);


			for(i = 0; i < len ; i++){


				fd->S_buff[fd->last_write % BUFFER_SIZE] = buff[i];
				//dbg(TRANSPORT_CHANNEL,"DATA SENDING???:%hhu\n", buff[i]);

				fd->last_write++;

			}
		// data left to be transmitted
		//WORK_LEFT -= len;
			TCP->SeqNum = fd->last_send;
			TCP->Flag             = DAT;
			TCP->Acknowledge      = ++fd->last_ack;
			TCP->SrcPort          = fd->Src_port;
			TCP->DstPort          = fd->Dest_port;
			TCP->length           = len;

			for(i = 0; i < min(EFF,MAX_PAYLOAD,len) ; i++){

				TCP->DATA[i] = fd->S_buff[fd->last_send % BUFFER_SIZE];

				fd->last_send++;

			}



			if(fd->last_write > fd->last_ack){ //normal

				TCP->AdvertisedWindow = ((fd->last_read-1)- fd->last_recieve);
			}

			else{ //wrap around

				TCP->AdvertisedWindow = BUFFER_SIZE - ((fd->last_recieve-1)- fd->last_read);
			}

			//dbg(TRANSPORT_CHANNEL, "SENDING DATA%d\n", fd->Dest_port);
			retranmission(TCP->Flag, fd->Dest_addr, TCP->SeqNum , MSG , fd);

			//call transSender.send(MSG, fd->Dest_addr);


		}

        return len;

	}


    command error_t Transport.receive(pack* package){
		//transmission reliable;
		Transfer * tcp = (Transfer*)package->payload;

		//					serverport 123    client 200 eg        // client addr
		uint32_t   key_src;
		socket_t * FD_src;
		//uint16_t   timer, sent;

		if(tcp->Flag == SYN && tcp->DstPort == SERVER_PORT){

			uint32_t new_key;
			uint32_t key = tcp->DstPort << 24 | SENTINEL << 16 | SENTINEL;
			socket_t * server_soc  = call Hash.get(key); // get the listen socket
			TCP = (Transfer*) MSG.payload;

			dbg(TRANSPORT_CHANNEL, "SYN  to server\n");

			//					//SERVER_PORT		//client port       //cleint address
			new_key = SERVER_PORT << 24 | tcp->SrcPort << 16 | package->src;

			if(call Hash.contains(new_key) == TRUE){

				if(!call Pool.empty()){

					server_soc = call Pool.get();

				}
				else{

					return FAIL;

				}

			}

			//dbg(TRANSPORT_CHANNEL, "Connect accept???%d %d %d\n", server_soc->Src_port, server_soc->Dest_port , server_soc->Dest_addr);
			server_soc->state     = SYN_RCVD;
			server_soc->Src_port  = SERVER_PORT;
			server_soc->Dest_port = tcp->SrcPort;
			server_soc->Dest_addr = package->src;
			call Hash.insert(new_key, server_soc);



			TCP->Flag = SYN_ACK;
			TCP->SeqNum =10;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SrcPort = SERVER_PORT;
			TCP->DstPort = tcp->SrcPort ;
			TCP->AdvertisedWindow = BUFFER_SIZE;

			server_soc->next_expected  = TCP->SeqNum +1;
			server_soc->last_recieve   = tcp->SeqNum;
			server_soc->last_read      = tcp->SeqNum;

			//dbg(TRANSPORT_CHANNEL, "POINTERS SERVER1 %hhu %hhu %hhu\n", server_soc->next_expected , server_soc->last_recieve, server_soc->last_read );

			//RETRANMISSION

			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG , server_soc);
			//return SUCCESS;

			return SUCCESS;

		}

		key_src = tcp->DstPort << 24 | tcp->SrcPort << 16 | package->src;

		if(call Hash.contains(key_src) == TRUE){

			FD_src = call Hash.get(key_src);
		}
		//else{
		//	dbg(TRANSPORT_CHANNEL, "DNE\n");
		//	return FAIL;
		//}

		if(tcp->Flag == (SYN_ACK) && FD_src->state == SYN_SEND){
			dbg(TRANSPORT_CHANNEL, "SYN + ACK to client\n");

			TCP = (Transfer*) MSG.payload;
			FD_src->state = EST;

			FD_src->last_ack   = tcp->Acknowledge;
			FD_src->last_send  = tcp->SeqNum;
			FD_src->last_write = tcp->SeqNum;


			TCP->Flag = ACK;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SeqNum = tcp->Acknowledge +1;
			TCP->SrcPort = tcp->DstPort;
			TCP->DstPort = SERVER_PORT;
			TCP->AdvertisedWindow = BUFFER_SIZE;

			signal Transport.connectDone(FD_src);
			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);
			//signal Transport.connectDone(FD_src);

			//RETRANMISSION


			return SUCCESS;
		}

		if(tcp->Flag == ACK && FD_src->state == SYN_RCVD){ //state
			dbg(TRANSPORT_CHANNEL, "ACK to server\n");
			FD_src->state = EST;

				FD_src->next_expected  = TCP->SeqNum +1;
				FD_src->last_recieve   = tcp->SeqNum;
				FD_src->last_read      = tcp->SeqNum;

			//CONFIRM = TRUE;
			dbg(TRANSPORT_CHANNEL, "HANDSHAKE complete \n");

			///signal accept
			signal Transport.accept(FD_src);
			return SUCCESS;
		}



		if(tcp->Flag == DAT && FD_src->state == EST){
			//question MAX_PAYLOAD_SIZE vs BUFFER_SIZE
			uint16_t i=0;
			dbg(TRANSPORT_CHANNEL, "recieve	%hhu    Next expected  %hhu\n",FD_src->last_recieve,FD_src->next_expected );


			FD_src->last_read = FD_src->last_recieve;
			for(i = 0; i < tcp->length; i++){// write to read buff

			 	FD_src->R_buff[(FD_src->last_recieve) % BUFFER_SIZE] = tcp->DATA[i];
				//dbg(TRANSPORT_CHANNEL, "SERVER DAT %hhu\n",tcp->DATA[i]);
				++FD_src->last_recieve;
				//FD_src->last_read++;
			}
			FD_src->next_expected = FD_src->last_recieve;


			TCP->Flag = DAT_ACK;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SeqNum  = tcp->Acknowledge +1;
			TCP->SrcPort = tcp->DstPort;
			TCP->DstPort = package->src;

			TCP->AdvertisedWindow = tcp->AdvertisedWindow;

			//RETRANMISSION
			//dbg(TRANSPORT_CHANNEL, "SENDING to CLIENT\n");
			//retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);
			call transSender.send(MSG,package->src);
			return SUCCESS;
		}

		if(tcp->Flag == DAT_ACK && FD_src->state== EST){

			dbg(TRANSPORT_CHANNEL, "DATA ACK   \n");
			FD_src->last_ack   = tcp->Acknowledge;
			FD_src->last_send  = tcp->SeqNum;
			FD_src->last_write = tcp->SeqNum;

			dbg(TRANSPORT_CHANNEL, "L A %hhu\n",FD_src->last_ack);
			dbg(TRANSPORT_CHANNEL, "LAST S %hhu\n",FD_src->last_send);
			dbg(TRANSPORT_CHANNEL, "LAST WR%hhu\n",FD_src->last_write);
			//dbg(TRANSPORT_CHANNEL, "POINTERS server %hhu %hhu %hhu\n", FD_src->next_expected , FD_src->last_recieve, FD_src->last_read );
			return SUCCESS;
		}


		//----------------------CLOSING-----------------------------------------

		if(tcp->Flag == FIN  && FD_src->state == EST){

			dbg(TRANSPORT_CHANNEL, "FIN to close 1 : %hhu  %hhu %hhu \n", tcp->DstPort,tcp->SrcPort, package->src);

			TCP = (Transfer*) MSG.payload;
			FD_src->state = CLOSED_WAIT;
			TCP->Flag = ACK;
			TCP->Acknowledge = tcp->SeqNum + 1;
			// TCP->SeqNum = ++tcp->Acknowledge;
			TCP->SrcPort = tcp->SrcPort;
			TCP->DstPort = tcp->DstPort;

			//dbg(TRANSPORT_CHANNEL, "timer runn  %hhu\n",call oneShot.isRunning());

			//RETRANMISSION
			//call Transport.close(FD_src);
			//retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);
			//call transSender.send(MSG, package->src);

			//call oneShot.stop();
			//call transSender.send(MSG, package->src);

			//dbg(TRANSPORT_CHANNEL, "timer runn  %hhu\n",call oneShot.isRunning());
			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);

			//call Transport.close(FD_src);





			return SUCCESS;
		}

		/*if(tcp->Flag == ACK && FD_src->state == FIN_WAIT){//server state closing

			dbg(TRANSPORT_CHANNEL, "ACK to close 1..%hhu  %hhu %hhu \n", tcp->DstPort,tcp->SrcPort, package->src);
			FD_src->state = FIN_WAIT_2;
//			return SUCCESS;

		}

		if(tcp->Flag ==  FIN && FD_src->state == FIN_WAIT_2){ //client state

			dbg(TRANSPORT_CHANNEL, "FIN_2 to close : %hhu  %hhu %hhu \n", tcp->DstPort,tcp->SrcPort, package->src);
			FD_src->state = TIME_WAIT;
			TCP = (Transfer*) MSG.payload;
			FD_src->state = TIME_WAIT;
			TCP->Flag = ACK;
			TCP->SeqNum = tcp->SeqNum + 1;
			// TCP->SeqNum = ++tcp->Acknowledge;
			TCP->SrcPort = tcp->SrcPort;
			TCP->DstPort = tcp->DstPort;

			//RETRANMISSION
			//retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);

			//ativate TIME_WAIT TIMER

			if(call WaitTimer.isRunning() == FALSE){

				call WaitTimer.startOneShot(TIME_WAIT);

			}

			FD_src->state = CLOSED;

			if(call Hash.contains(key_src) == TRUE){
				call Hash.remove(key_src);
				call Pool.put(FD_src);
				dbg(TRANSPORT_CHANNEL, "socket deAllocked\n");
				//return SUCCESS;
			}
			else{
				dbg(TRANSPORT_CHANNEL, "socket NOT deAllocked\n");
				return FAIL;
			}
		}


		if(tcp->Flag == ACK && FD_src->state == CLOSED_WAIT){  //server state closing

			dbg(TRANSPORT_CHANNEL, "ACK to closed 2: %hhu  %hhu %hhu \n", tcp->DstPort,tcp->SrcPort, package->src);
			FD_src->state = CLOSED;

			dbg(TRANSPORT_CHANNEL, "deAllocking socket\n");


			if(call Hash.contains(key_src) == TRUE){
				call Hash.remove(key_src);
				call Pool.put(FD_src);
				dbg(TRANSPORT_CHANNEL, "socket deAllocked\n");
			}
			else{
				dbg(TRANSPORT_CHANNEL, "socket NOT deAllocked\n");
				return FAIL;
			}

		}*/

        return SUCCESS;
	}

	event void oneShot.fired(){
		uint16_t dt,sent;

		///pointer maybe
		transmission transmiting = call Queue.dequeue();
		//dbg(TRANSPORT_CHANNEL, "SIZE %hhu\n",call Queue.size());

	//dbg(TRANSPORT


		if(!call Queue.empty()){  //MSG.seq

			if(transmiting.fd->last_ack < transmiting.fd->last_send ){

				if((transmiting.fd->last_ack > transmiting.SeqNum || transmiting.SeqNum > transmiting.fd->last_send)){ // it was not ACKED

					sent = call oneShot.getNow();
					transmiting.time =  sent + RTT;
					transmiting.drop +=1;
					call Queue.enqueue(transmiting);

					call transSender.send(MSG,transmiting.dest);

				}
			}

			//if(transmiting.fd->last_ack > transmiting.fd->last_send )

			else{

				if(transmiting.fd->last_ack < transmiting.SeqNum && transmiting.SeqNum < transmiting.fd->last_send){
					sent = call oneShot.getNow();
					transmiting.time =  sent + RTT;
					//transmiting.fd =
					transmiting.drop +=1;
					call Queue.enqueue(transmiting);

					call transSender.send(MSG,transmiting.dest);

				}
			}
		}

		if(!call Queue.empty()){
			transmiting = call Queue.head();

			if(transmiting.time <call oneShot.getNow()){

				call oneShot.startOneShot(100);
			}

			if(transmiting.time > call oneShot.getNow()){
				dbg(TRANSPORT_CHANNEL, "pack lost.... RETRANMISSION\n");
				dt = transmiting.time -  call oneShot.getNow();

				call oneShot.startOneShot(dt);

			}

		}


	//	dbg(TRANSPORT_CHANNEL, "size %hhu    SENT TO  %hhu    Flag %hhu    SeqNum %hhu     time  %hhu    drop %hhu\n", call Queue.size(),transmiting.dest,transmiting.type ,transmiting.SeqNum,transmiting.time, transmiting.drop);


	}


    command error_t Transport.connect(socket_t *fd, socket_addr_t * addr){
		//client side                                             server addr
		//create socket with the socket();
		//connect the socket to the addr of the server using connect()
		//send and recieve data using write and read call repectively
		//A connection is a combination of a (source and dest) -> [address and port value].
		//An active open is the creation of a connection to a listening port by a client. It uses socket() and connect().

		uint32_t KEY;

		//uint16_t dt,sent;
		//transmission transmit;


		dbg(TRANSPORT_CHANNEL, "CONNECTION start() \n");

		fd->Src_port  = getPort(); // that retun port 0-255
		fd->state = SYN_SEND;//change the sate after we send the SYN

		fd->Dest_port = addr->source_Port;
		fd->Dest_addr = addr->source_Addr;

		//send() stage;

		TCP = (Transfer*) MSG.payload;
		TCP->Flag = SYN;
		TCP->SeqNum = 0;
		TCP->Acknowledge = 0;
		TCP->AdvertisedWindow = BUFFER_SIZE;
		TCP->SrcPort = fd->Src_port;//random asumm 200
		TCP->DstPort = addr->source_Port; //123

		fd->last_ack   = TCP->SeqNum +1;
		fd->last_send  = TCP->SeqNum;
		fd->last_write = TCP->SeqNum;

		//		//200				//123					//1
		KEY = fd->Src_port << 24 | addr->source_Port << 16 | addr->source_Addr;

		dbg(TRANSPORT_CHANNEL, "SENDING... \n");

		call Hash.insert(KEY,fd);

		//dbg(TRANSPORT_CHANNEL, "SENDING... \n");
		retranmission(TCP->Flag, SERVER_ADDR, TCP->SeqNum , MSG ,fd);


		//dbg(TRANSPORT_CHANNEL, "connect() SUCCESS... \n");
        return SUCCESS;
    }

    command error_t Transport.close(socket_t* fd){
		//closes a file descriptor, so that it no longer refers to any file and may be reused

		//To deallocate means to make the file descriptor available for return by
		//subsequent calls to open() or other functions that allocate file descriptors.

		//change the state and send the initallly FIN reliable.



		if(fd->state == EST){
			uint32_t key;
			dbg(TRANSPORT_CHANNEL, "work done...FIN...\n");
			//dbg(TRANSPORT_CHANNEL, "SRcport %hhu  destport %hhu destaddr %hhu\n",fd->Src_port, fd->Dest_port, fd->Dest_addr);
			TCP = (Transfer*) MSG.payload;
			TCP->Flag = FIN;
			fd->state = FIN_WAIT;
			TCP->SeqNum = TCP->Acknowledge;

			retranmission(TCP->Flag, fd->Dest_addr, TCP->SeqNum , MSG ,fd);

    	}

		if(fd->state == CLOSED_WAIT){//server close
			uint32_t key;
			dbg(TRANSPORT_CHANNEL, "server close\n");
			TCP = (Transfer*) MSG.payload;
			TCP->Flag = FIN ;
			fd->state = LAST_ACK;
			TCP->SeqNum = TCP->Acknowledge;

			retranmission(TCP->Flag, fd->Dest_addr, TCP->SeqNum , MSG ,fd);

		}

		return SUCCESS;
	}

    command error_t Transport.release(socket_t* fd){
		/*
		TCP = (Transfer*) MSG.payload;
		TCP->Flag = FIN;
		//FIN flag indicates that the byte before sequence number x was the last byte of the byte stream sent by the remote host.
		TCP->SeqNum = (Transfer*) fd->last_S_send ;
		call transSender.send(MSG, fd->Dest_addr);
		*/
        return SUCCESS;
    }


    command error_t Transport.listen(socket_t * fd){

		//A passive open is the creation of a listening socket, to accept incoming connections.
		//It uses socket(), bind(), followed by an accept() loop.

		//The listen operation then defines how many connections can be pending on the specified socket
		//fd->Dest_addr = SENTINEL;// make sentinel value
		//fd->Dest_port = SENTINEL;

		uint32_t key;

		fd->state     = LISTEN;
		fd->Dest_addr = SENTINEL;// make sentinel value
		fd->Dest_port = SENTINEL;
		fd->Src_port = SERVER_PORT; //123

		//		//123 			//999 					//999
		key = fd->Src_port<< 24| fd->Dest_port << 16 | fd->Dest_addr ;

		call Hash.insert(key,fd);

        return SUCCESS;
    }




	event void WaitTimer.fired(){ //signals that the timer has expired (one-shot) or repeated (periodic).
		dbg(TRANSPORT_CHANNEL, "timer runn  %hhu\n",1000);
		call transSender.send(MSG, 4);
	}


	void Socketlog(socket_t * input){

		dbg(TRANSPORT_CHANNEL,"Socket Info - Src: %hhu Dest: %hhu Addr: %hhu ACK: %hhu Rcvd: %hhu Written:%hhu Next_expected:%hhu last_read:%hhu last_send:%hhu \n",
		input->Src_port, input->Dest_port, input->Dest_addr, input->last_ack, input->last_recieve,
		input->last_write,input->next_expected, input->last_read, input->last_send);

	}

	uint8_t min(uint8_t EF,uint8_t PAYLOAD, uint8_t data_it){

		uint8_t c = 0;
		 while ( EF &&  PAYLOAD && data_it){

      		EF--;  PAYLOAD--; data_it--; c++;

  		}
  		return c;

	}


	uint8_t min2(uint8_t EF,uint8_t PAYLOAD){

		if(EF < PAYLOAD){
			return EF;
		}
		else{

			return PAYLOAD;
		}

	}

	uint8_t getPort(){
		uint8_t temp = 0;
		uint16_t i = 0;

		if(once == FALSE){
			filler ();
			once = TRUE;
		}

		temp = rand()% 255;

		for (i = 0 ; i < 255; i++){// use to fill up neighborList w/place holder value

			if(ports[i]== SENTINEL){
				continue;
			}

			if(temp == ports[i] || ports[i] == 80){
				temp = rand()% 255;
				i=0;//recheck
			}


		}

		ports[port_count++]= temp;

		return temp;

	}


	void Free_port(uint8_t port){

		uint16_t i = 0;

		for (i = 0 ; i < 255; i++){// use to fill up neighborList w/place holder value

			if(port == ports[i]){

				ports[i] = SENTINEL;

			}


		}


	}

	void filler (){ // use to fill the neighborList with the known NULL value place holder
		uint16_t i = 0;

		for (i = 0 ; i < 255; i++){// use to fill up neighborList w/place holder value

			ports[i]  =  SENTINEL;


		}

	}



	default event void * Transport.accept(socket_t * fd){}

	///event message_t* packetReceived.receive(message_t* msg, void* payload, uint8_t len){return msg;}
}


/*reliable.type   = TCP->Flag;
reliable.dest   = package->src;
reliable.SeqNum = TCP->SeqNum;

sent = call oneShot.getNow();
reliable.time   =  sent + RTT;
reliable.fd = server_soc;
call Queue.enqueue(reliable);
dbg(TRANSPORT_CHANNEL, "SYN  to server@\n"); call transSender.send(MSG, package->src);

timer = call oneShot.getNow() + (sent + (2* RTT));

if(call oneShot.isRunning() == FALSE){

	call oneShot.startOneShot(timer);

}*/


	/*event message_t* InternalReceived.receive(message_t* msg, void* payload, uint8_t len){

		pack* Msg = (pack*) payload;

		dbg(ROUTING_CHANNEL,"Node REACHED... recieve.....\n");

		if(Msg->dest == TOS_NODE_ID){

			if(Msg->protocol == PROTOCOL_TCP){

				//dbg(ROUTING_CHANNEL,"Node REACHED... recieve.....\n");
				call Transport.receive(Msg);

				return signal MainReceive.receive(msg, payload, len);

			}
			else{
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

	}*/







	/*multitimer:  timer.time()
		queue.insert(time+30k);

		t = queue.dequeue()     FIFO

		delta time = time() -t

		start.oneShot(delta time)

	*/

	/*
		FLAGS bit manipluation

		syn  = 1
		ack  = 2
		data = 4

		-------------------

		-------------------
	*/

	/*

		gona need another timer for the handshake

	*/

	//threeway handshake
	//client     server
	//syn->
	//<-syn + ack
	//ack->


	//host name node
	//addr =255


	//client side  read data and print
	//server side write an send
