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


/*
ubuntu@ubuntu-xenial:~/CSE160/CSE160-Project-Skeleton-Code$ python pingTest.py
Traceback (most recent call last):
  File "pingTest.py", line 1, in <module>
    from TestSim import TestSim
  File "/home/ubuntu/CSE160/CSE160-Project-Skeleton-Code/TestSim.py", line 7, in <module>
    from TOSSIM import *
  File "/home/ubuntu/CSE160/CSE160-Project-Skeleton-Code/TOSSIM.py", line 7, in <module>
    import _TOSSIM
ImportError: /home/ubuntu/CSE160/CSE160-Project-Skeleton-Code/_TOSSIMmodule.so: cannot map zero-fill pages
*/
module TransportP{
	/// uses interface
	uses interface Timer<TMilli> as WaitTimer;
	uses interface Timer<TMilli> as oneShot;
	uses interface Timer<TMilli> as EFF_timer;

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

	uint8_t stop;

	uint8_t WORK_LEFT =WORK_DONE;
	uint8_t l=1;
	void filler ();
	uint8_t getPort();
	void Free_port (uint8_t port);
	void Socketlog(socket_t * input);
	uint8_t min(uint8_t EF,uint8_t PAYLOAD);


	void retranmission(uint8_t Flag, uint16_t dest,uint16_t SeqNum,pack msg, socket_t *fd){


		uint16_t dt,sent;
		transmission transmit;
		transmit.type   = Flag;
		transmit.dest   = dest;
		transmit.SeqNum = SeqNum;

		sent = call oneShot.getNow();
		transmit.time =  sent + RTT;
		transmit.fd   = fd;
		call Queue.enqueue(transmit);

		//dbg(TRANSPORT_CHANNEL, "RETRANMISSION : l_write %hhu  i %hhu  \n",Flag, l++);


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

	uint8_t match (char text[], uint8_t txtlen, char pattern[], uint8_t patlen) {
	    uint8_t c, d, e, text_length, pattern_length, position = 0;

	    text_length = txtlen;
	    pattern_length = patlen;

	    if (pattern_length > text_length) {
	        return 0;
	    }

	    for (c = 0; c < text_length - pattern_length; c++) {
	        position = e = c;

	        for (d = 0; d < pattern_length; d++) {
	            if (pattern[d] == text[e]) {
	                e++;
	            }
				else {
	                break;
	            }
	        }
	        if (d == pattern_length) {
	            return position;
	        }
	    }

	    return 0;
	}

	command void Transport.print (uint8_t *buff,uint16_t start, uint16_t bufflen, bool type){
		//R_buff [BUFFER_SIZE]
		uint8_t i=0;

		uint16_t a,b,c,d,e ,s,f, l,h,m;

		uint16_t response_type=0;
		uint16_t htrack=0;
		uint16_t ltrack=0;
		uint16_t mtrack=0;
		uint8_t print[128];
		char rev [bufflen];
		bool t = FALSE;
		char H []= "olleh"; //5
		char M []= "egassem";//7
		char L []= "sresutsil";//9
			// 1 hello
			//2 message
			//3 list

			//dbg(GENERAL_CHANNEL, "R2466rdcata\n ");

		i = bufflen;
		a =0;
		while(i > 0){

			//dbg(GENERAL_CHANNEL, "Reading data %c\n", buff[i]);
			rev[a] = buff[i];
			i--;
			a++;
		}

		l = match(rev,bufflen, L,9);
		//dbg(GENERAL_CHANNEL, "size %hhu\n");
		if(l > 0){
			l +=  8;
			while(l> 0){

				if(rev[l] > 0 && rev[l] < 128){
					dbg(GENERAL_CHANNEL, "Reading data %c\n", rev[l]);
				}
				l--;
			}

		}

		if(l==0){
			m = match(rev,bufflen, M ,7);
			if(m > 0){

				m +=  6;
				while(m> 0){

					if(rev[m] > 0 && rev[m] < 128){
						dbg(GENERAL_CHANNEL, "Reading data %c\n", rev[m]);
					}
					if(rev[m] ==13){
						break;

					}
					m--;
				}

			}

		}
		if(m ==0){
			h= match(rev,bufflen, H,5);

			if(h > 0){

				h +=  4;
				while(h> 0){

					dbg(GENERAL_CHANNEL, "Reading data %c\n", rev[h]);
					h--;
				}

			}

		}


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
		uint8_t len=0,i,z;

		//dbg(TRANSPORT_CHANNEL, "last_read: %hhu last_recieve %hhu  last_next %hhu \n",fd->last_read,fd->last_recieve, fd->next_expected);

	//dbg(TRANSPORT_CHANNEL, "SERVER DATz %hhu\n",fd->Src_port);
		if(fd->state == EST){

			if(fd->last_recieve%BUFFER_SIZE >= fd->last_read%BUFFER_SIZE){ //normal case

				if(bufflen <= (fd->last_recieve%BUFFER_SIZE - fd->last_read%BUFFER_SIZE)  ){ //TAKE ALL OF IT
					len = bufflen;
					//dbg(TRANSPORT_CHANNEL,"DATA a: len %hu  \n", len );
				}
				else{

					len =  (fd->last_recieve%BUFFER_SIZE - fd->last_read%BUFFER_SIZE);
					//dbg(TRANSPORT_CHANNEL,"DATA b: len %hu  \n", len );

				}
			}

			else{ //wrap around

				if(bufflen <= (fd->last_read%BUFFER_SIZE - fd->last_recieve%BUFFER_SIZE)  ){ //TAKE ALL OF IT

					len = bufflen;
					//dbg(TRANSPORT_CHANNEL,"DATA c: len %hu  \n", len );
				}
				else{

					len = BUFFER_SIZE  - (fd->last_read%BUFFER_SIZE - fd->last_recieve%BUFFER_SIZE);
					//dbg(TRANSPORT_CHANNEL,"DATA d: len %hu  \n", len );

				}
			}


			for(i = 0; i < len ; i++){


				buff[i]= fd->R_buff[(fd->last_read)% BUFFER_SIZE];
				//dbg(TRANSPORT_CHANNEL, "ValueZ:%c \n", buff[i]);

				//dbg(TRANSPORT_CHANNEL, "SERVER DAT %c\n", ((fd->R_buff[i+1]<< 8 ) | (fd->R_buff[i] & 0xff)) );
				fd->last_read++;

			}

			//dbg(TRANSPORT_CHANNEL, "READ: %hhu  %hhu\n",TCP->AdvertisedWindow,len );

			//TCP->AdvertisedWindow += len;

			//dbg(TRANSPORT_CHANNEL, "READ@: %hhu\n",TCP->AdvertisedWindow );
		}

        return len;
    }

	event void EFF_timer.fired(){ //signals that the timer has expired (one-shot) or repeated (periodic).

	}



    command uint16_t Transport.write(socket_t* fd, uint8_t *buff, uint16_t bufflen){
		//write to the send buffer
		uint8_t len,i,to_write, data_left, EFF, save,z ;
		uint16_t a;
		TCP = (Transfer*) MSG.payload;



		if(fd->state == EST){
			//dbg(TRANSPORT_CHANNEL,"DATA W: write %hhu ACk %hhu  send %hhu  dest port %hhu\n",fd->last_write, fd->last_ack, fd->last_send, fd->Dest_port);

			if(fd->last_write%BUFFER_SIZE >= fd->last_ack%BUFFER_SIZE){ //wrap around

				if(bufflen <= (BUFFER_SIZE -(fd->last_write%BUFFER_SIZE - fd->last_ack%BUFFER_SIZE) ) ){ //TAKE ALL OF IT
					len = bufflen;
					//dbg(TRANSPORT_CHANNEL,"DATA a: len %hhu  \n", len );
				}

				else { // TAKE A PORTION
					len = (BUFFER_SIZE-(fd->last_write%BUFFER_SIZE - fd->last_ack%BUFFER_SIZE)); //take all the remaining buffer
					//dbg(TRANSPORT_CHANNEL,"DATA b: len %hhu  \n", len );
				}

			}

			else{ //typical

				if(bufflen <= (fd->last_ack%BUFFER_SIZE - fd->last_write%BUFFER_SIZE)){ //TAKE ALL OF IT
					len = bufflen;
					//dbg(TRANSPORT_CHANNEL,"DATA c: len %hhu  \n", len );
				}

				else{ // TAKE A PORTION
					len = (fd->last_ack%BUFFER_SIZE - fd->last_write%BUFFER_SIZE); //take all the remaining buffer
					//dbg(TRANSPORT_CHANNEL,"DATA d: len %hhu  \n", len );
				}

			}

			//dbg(TRANSPORT_CHANNEL, "SERVER DATz %hhu \n",len);



			for(i = 0; i < 12; i++){

				fd->S_buff[fd->last_write % BUFFER_SIZE] = buff[i];

				//dbg(TRANSPORT_CHANNEL, "ValueZ:%c \n", buff[i]);
				fd->last_write++;

				if(buff[i] == '\n'){

					break;
				}

			}


			//dbg(TRANSPORT_CHANNEL,"DATA SENDING??:%hhu\n", fd->S_buff[fd->last_send % BUFFER_SIZE] );
			EFF = TCP->AdvertisedWindow;// -  (fd->last_send- fd->last_ack);


			if(EFF < 1){

				dbg(TRANSPORT_CHANNEL,"eff win size:%hu\n", EFF);

				if(call EFF_timer.isRunning() == FALSE){

					call EFF_timer.startOneShot(600);

				}

			}


			TCP->Flag             = DAT;
			TCP->SeqNum           = fd->last_send;
			TCP->SrcPort          = fd->Src_port;
			TCP->DstPort          = fd->Dest_port;
			TCP->length           = len;

			//dbg(TRANSPORT_CHANNEL, "SERVER DATz %hhu \n",min(EFF, MAX_PAYLOAD));


			for(i = 0; i < min(EFF, MAX_PAYLOAD); i++){

				TCP->DATA[i] = fd->S_buff[fd->last_send  % BUFFER_SIZE];

				fd->last_send++;
				if(buff[i] == '\n'){

					break;
				}

			}

			//if(EFF > 0){
				retranmission(TCP->Flag, fd->Dest_addr, TCP->SeqNum , MSG , fd);
			//}

			return len;
		}


	}


    command error_t Transport.receive(pack* package){
		//transmission reliable;
		Transfer * tcp = (Transfer*) &package->payload;

		//					serverport 123    client 200 eg        // client addr
		uint32_t   key_src;
		socket_t * FD_src;
		//uint16_t   timer, sent;

		if(tcp->Flag == SYN && tcp->DstPort == SERVER_PORT){

			uint32_t new_key;
			//uint32_t key = tcp->DstPort << 24 | SENTINEL << 16 | SENTINEL;
			socket_t * server_soc;// get the listen socket

			//					//SERVER_PORT		//client port       //cleint address
			new_key = SERVER_PORT << 24 | tcp->SrcPort << 16 | package->src;

			if(call Hash.contains(new_key) == FALSE){

				if(!call Pool.empty()){
					server_soc = call Pool.get();
					call Hash.insert(new_key, server_soc);
				}
				else{
					return FAIL;
				}
			}
			else{
				return FAIL;
			}
			dbg(TRANSPORT_CHANNEL, "SYN  to server %hu  %hhu\n", tcp->SeqNum, server_soc->last_recieve);

			///dbg(TRANSPORT_CHANNEL, "Connect accept???%d %d %d\n", server_soc->Src_port, server_soc->Dest_port , server_soc->Dest_addr);
			server_soc->state     = SYN_RCVD;

			server_soc->Src_port  = SERVER_PORT;
			server_soc->Dest_port = tcp->SrcPort;
			server_soc->Dest_addr = package->src;


			TCP = (Transfer*) MSG.payload;

			TCP->Flag = SYN_ACK;
			TCP->SeqNum =0;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SrcPort = tcp->DstPort;//SERVER
			TCP->DstPort = tcp->SrcPort;
			TCP->AdvertisedWindow = BUFFER_SIZE;


			// no data has been recived
			server_soc->next_expected  = TCP->SeqNum +1;
			server_soc->last_recieve   = tcp->SeqNum;
			server_soc->last_read      = tcp->SeqNum;

			server_soc->last_send  =  tcp->SeqNum +1 ;
			server_soc->last_ack   = tcp->SeqNum +1;
			server_soc->last_write = tcp->SeqNum +1;


			//dbg(TRANSPORT_CHANNEL, "SERVER   sin %hhu   ak%hhu  \n",TCP->SeqNum  ,TCP->Acknowledge );
			//dbg(TRANSPORT_CHANNEL, "SERVER   next %hhu   rcvd %hhu  read%hhu  \n",server_soc->next_expected ,server_soc->last_recieve,server_soc->last_read );

			//RETRANMISSION
			//call transSender.send(MSG, package->src);
			//retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG , server_soc);
			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG , server_soc);

			return SUCCESS;
		}

		key_src = tcp->DstPort << 24 | tcp->SrcPort << 16 | package->src;
		FD_src = call Hash.get(key_src);

		if(tcp->Flag == (SYN_ACK) && FD_src->state == SYN_SEND){

			dbg(TRANSPORT_CHANNEL, "SYN + ACK to client\n");

			TCP = (Transfer*) MSG.payload;
			FD_src->state = EST;

			TCP->Flag = ACK;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SeqNum      = tcp->Acknowledge;
			//dbg(TRANSPORT_CHANNEL, "Connect done  %hhu  %hhu\n",tcp->Acknowledge, tcp->SeqNum );

			TCP->SrcPort = tcp->DstPort;
			TCP->DstPort = tcp->SrcPort;
			TCP->AdvertisedWindow = BUFFER_SIZE;

			FD_src->last_send  = TCP->SeqNum;
			FD_src->last_ack   = TCP->Acknowledge;
			FD_src->last_write = TCP->SeqNum;

			FD_src->last_recieve  = tcp->SeqNum;
			FD_src->next_expected  = tcp->SeqNum + 1;
			FD_src->last_read     = tcp->SeqNum;

			//dbg(TRANSPORT_CHANNEL, "CLIENT   sin %hhu   ak%hhu  \n",TCP->SeqNum  ,TCP->Acknowledge );
			//dbg(TRANSPORT_CHANNEL, "client   next %hhu   rcvd %hhu  read%hhu  \n",FD_src->last_send ,FD_src->last_ack ,	FD_src->last_write );


			//dbg(TRANSPORT_CHANNEL, "Connect done%d %d %d\n", FD_src->Src_port, FD_src->Dest_port , FD_src->Dest_addr);
			//dbg(TRANSPORT_CHANNEL, "CLIENT: send %hhu   SeqNum %hu   ack  %hu   write  %hu\n",FD_src->last_send, TCP->SeqNum ,FD_src->last_ack ,FD_src->last_write);
			//dbg(TRANSPORT_CHANNEL, "POINTERS CLIENT2  %hhu %hhu %hhu\n",FD_src->last_ack, FD_src->last_send, FD_src->last_write);
			signal Transport.connectDone(FD_src);
			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);
			//call transSender.send(MSG, package->src);

			//RETRANMISSION

			return SUCCESS;
		}

		if(tcp->Flag == ACK && FD_src->state == SYN_RCVD){ //state
			dbg(TRANSPORT_CHANNEL, "ACK to server\n");
			FD_src->state = EST;
			//dbg(TRANSPORT_CHANNEL, "Connect done  %hhu  %hhu\n",tcp->SeqNum , tcp->Acknowledge);


			if(FD_src->next_expected == tcp->SeqNum){

				dbg(TRANSPORT_CHANNEL, "HANDSHAKE complete \n");
				//dbg(TRANSPORT_CHANNEL, "SERVER??  SeqNum %hhu   N exp %hhu  l rcvd %hhu   l read%hhu\n",tcp->SeqNum, FD_src->next_expected, FD_src->last_recieve, FD_src->last_read);

				///signal accept
				signal Transport.accept(FD_src);
				return SUCCESS;
			}
			else{
				return FAIL;
			}


		}

		if(tcp->Flag == DAT && FD_src->state == EST){
			uint16_t i=0,z,a,b,fal=0;
			uint16_t zero_trakcer[128];


			/*
			zero_trakcer[i] = ((tcp->DATA[i+1]<< 8 ) | (tcp->DATA[i] & 0xff)) ;
			dbg(TRANSPORT_CHANNEL, "SERVER DATz %c\n",zero_trakcer[i]);

			if(zero_trakcer[i] != 0){

				a= (zero_trakcer[i] & 0xff) ;
				b= (zero_trakcer[i]<< 8 );

				FD_src->R_buff[(FD_src->last_recieve++) % BUFFER_SIZE] = a;
				FD_src->R_buff[(FD_src->last_recieve++) % BUFFER_SIZE] = b;
			}

			*/

			if(FD_src->next_expected == tcp->SeqNum){
				dbg(TRANSPORT_CHANNEL, "recieve from %hhu \n",tcp->SrcPort );
				for(i = 0; i < tcp->length; i++){// write to read buff



				 	FD_src->R_buff[(FD_src->last_recieve) % BUFFER_SIZE] = tcp->DATA[i];
					//dbg(TRANSPORT_CHANNEL, "SERVER DATZZ %c\n", tcp->DATA[i] );

					//dbg(TRANSPORT_CHANNEL, "SERVER DAT %hhu  %hhu\n",tcp->DATA[i], FD_src->R_buff[(FD_src->last_recieve) % BUFFER_SIZE] );
					FD_src->last_recieve++;

					if(tcp->DATA[i] == '\n'){
						break;
					}


				}
				//dbg(TRANSPORT_CHANNEL, "SERVER DATz %hhu\n",FD_src->last_recieve);

				FD_src->next_expected = FD_src->last_recieve+1;

				TCP = (Transfer*) MSG.payload;
				TCP->Flag = DAT_ACK;
				TCP->Acknowledge = FD_src->next_expected%BUFFER_SIZE;

				TCP->SrcPort = tcp->DstPort;
				TCP->DstPort =  tcp->SrcPort;

				TCP->AdvertisedWindow = BUFFER_SIZE -((FD_src->next_expected -1)- FD_src->last_read);

				//RETRANMISSION
				//dbg(TRANSPORT_CHANNEL, "adwin %hu \n",TCP->AdvertisedWindow);
				//i=0;
				//while(i<128){zero_trakcer[i] =0; i++;}
				call transSender.send(MSG, package->src);

				return SUCCESS;

			}
			else{
				return FAIL;
			}

		}

		if(tcp->Flag == DAT_ACK && FD_src->state== EST){

			//FD_src->last_send  = tcp->Acknowledge;
			//if( tcp->Acknowledge > FD_src->last_ack){

				dbg(TRANSPORT_CHANNEL, "										DATA ACK %hhu  ack %hhu\n", tcp->Acknowledge,FD_src->last_ack );
				FD_src->last_ack   = tcp->Acknowledge;
				TCP->AdvertisedWindow = tcp->AdvertisedWindow;
			//}

			//FD_src->last_write = tcp->Acknowledge;

			//dbg(TRANSPORT_CHANNEL, "L A %hhu\n",FD_src->last_ack);
			//dbg(TRANSPORT_CHANNEL, "LAST S %hhu\n",FD_src->last_send);
			//dbg(TRANSPORT_CHANNEL, "LAST WR%hhu\n",FD_src->last_write);
			//dbg(TRANSPORT_CHANNEL, "POINTERS server %hhu %hhu %hhu\n", FD_src->next_expected , FD_src->last_recieve, FD_src->last_read );
			return SUCCESS;
		}


		//----------------------CLOSING-----------------------------------------

		if(tcp->Flag == FIN  && FD_src->state == EST){//server

			dbg(TRANSPORT_CHANNEL, "FIN 1 RCVD\n");
			TCP = (Transfer*) MSG.payload;
			FD_src->state = CLOSED_WAIT;
			TCP->Flag = ACK;

			TCP->Acknowledge = tcp->SeqNum + 1;
			dbg(TRANSPORT_CHANNEL, "													SRcport %hhu  \n",tcp->Acknowledge);
			TCP->SeqNum = tcp->Acknowledge;
			TCP->SrcPort = tcp->DstPort;
			TCP->DstPort = tcp->SrcPort;


			FD_src->next_expected  = tcp->SeqNum + 1;
			FD_src->last_recieve   = tcp->SeqNum;
			FD_src->last_read      = tcp->SeqNum;

			//RETRANMISSION
			//call Transport.close(FD_src);
			retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);

			call Transport.close(FD_src);


			return SUCCESS;

		}

		if(tcp->Flag == ACK && FD_src->state == FIN_WAIT){//client state closing

			dbg(TRANSPORT_CHANNEL, "ACK1  RCVD\n");
			FD_src->state = FIN_WAIT_2;

			TCP->Acknowledge = tcp->SeqNum + 1;
			//TCP->Acknowledge=
			//dbg(TRANSPORT_CHANNEL, "													SRcport %hhu  \n",tcp->Acknowledge);
			//dbg(TRANSPORT_CHANNEL, "													SRcport %hhu  \n",tcp->SeqNum);

			FD_src->last_ack   = tcp->Acknowledge;
			FD_src->last_send  = tcp->SeqNum;
			FD_src->last_write = tcp->SeqNum;


		}

		if(tcp->Flag ==  FIN && FD_src->state == FIN_WAIT_2){ //client state
			uint32_t * key = call Hash.getKeys();
			uint32_t  keyz= FD_src->Src_port << 24 | FD_src->Dest_port << 16 |  package->src;
			uint32_t i=0;
			dbg(TRANSPORT_CHANNEL, "FIN_2 RCVD\n");

			TCP = (Transfer*) MSG.payload;
			FD_src->state = TIME_WAIT;
			TCP->Flag = ACK;

			FD_src->last_ack   = tcp->Acknowledge;
			FD_src->last_send  = tcp->SeqNum;
			FD_src->last_write = tcp->SeqNum;

			TCP->SeqNum =  tcp->Acknowledge +1;
			TCP->Acknowledge = tcp->SeqNum + 1;
			TCP->SrcPort = tcp->DstPort;
			TCP->DstPort = tcp->SrcPort;

			//RETRANMISSION
			call transSender.send(MSG, package->src);
			//retranmission(TCP->Flag, package->src, TCP->SeqNum , MSG ,FD_src);

			//ativate TIME_WAIT TIMER

			if(call WaitTimer.isRunning() == FALSE){

				call WaitTimer.startOneShot(TIME_WAIT);

			}

			FD_src->state = CLOSED;

			for(i=0; i< call Hash.size(); i++){

				dbg(TRANSPORT_CHANNEL, "key %u  newkey  %u\n", key[i], keyz);
				if(key[i] == keyz){
					call Pool.put(FD_src);
					//call Hash.remove(key[i]);

				}

			}



			dbg(TRANSPORT_CHANNEL, "deAllocking socket: removing socket SrcPort %hhu  destPort %hhu destaddr %hhu\n", FD_src->Src_port, FD_src->Dest_port, package->src);


			return SUCCESS;

		}
		if(tcp->Flag == ACK && FD_src->state == LAST_ACK){  //server state closing
			uint32_t * key = call Hash.getKeys();
			uint32_t  keyz= FD_src->Src_port << 24 | FD_src->Dest_port << 16 |  package->src;
			uint32_t i=0;


			dbg(TRANSPORT_CHANNEL, "ACK 2 RCVD \n");
			FD_src->state = CLOSED;


			dbg(TRANSPORT_CHANNEL, "deAllocking socket: removing socket  SrcPort %hhu  destPort %hhu destaddr %u\n",  FD_src->Src_port,FD_src->Dest_port,package->src);


			for(i=0; i< call Hash.size(); i++){

				//dbg(TRANSPORT_CHANNEL, "key %u  newkey  %u\n", key[i], keyz);
					if(key[i] == keyz){

						call Pool.put(FD_src);
						//call Hash.remove(key[i]);

					}

				}
			return SUCCESS;

		}

        //return SUCCESS;
	}

	event void oneShot.fired(){
		uint16_t dt,sent,i=0;
		transmission transmiting;
		//HEAD-TIMER : flag 8 dest 1  SeqNum 101  last_ack  41 send 145  size 7 time 57404

		for(i =0; i < call Queue.size(); i++){
			transmiting = call Queue.head();

			if(transmiting.fd->last_ack <= transmiting.fd->last_send ){ //typicall

				if((transmiting.fd->last_ack >= transmiting.SeqNum || transmiting.SeqNum >= transmiting.fd->last_send)){
					call Queue.dequeue();
					i++;
					continue;
				}
			}
			else { //wrap

				if(transmiting.SeqNum <= transmiting.fd->last_ack  && transmiting.SeqNum >= transmiting.fd->last_send){
					call Queue.dequeue();
					i++;
					continue;
				}

			}
			break;

		}


		if(!call Queue.empty()){  //MSG.seq


			transmiting = call Queue.dequeue();
			//dbg(TRANSPORT_CHANNEL, "ack %hhu  sent %hhu  SeqNum %hhu\n",transmiting.fd->last_ack,transmiting.fd->last_send,transmiting.SeqNum );
			//dbg(TRANSPORT_CHANNEL, "HEAD-TIMER : flag %hu dest %hu  SeqNum %hu  last_ack  %hu send %hhu  size %hhu time %hu\n",
			//transmiting.type , transmiting.dest,transmiting.SeqNum,transmiting.fd->last_ack,transmiting.fd->last_send, call Queue.size() ,transmiting.time);
			//HEAD-TIMER : flag 8 dest 1  SeqNum 61  last_ack  33 send 113  size 19 tim
			//HEAD-TIMER : flag 2 dest 1  SeqNum 0  last_ack  25 send 141  size 13 time 53274

			if(transmiting.fd->last_ack <= transmiting.fd->last_send ){ //typicall
				//dbg(TRANSPORT_CHANNEL, "wrap\n ");

				if((transmiting.fd->last_ack >= transmiting.SeqNum || transmiting.SeqNum >= transmiting.fd->last_send)){
					//call Pool.put(FD_src);
					//put in pool
					//dbg(TRANSPORT_CHANNEL, "									ACKED 1\n ");
				}
				else{// it was not ACKED

					//dbg(TRANSPORT_CHANNEL, "NOT ACKED 1\n ");
					sent = call oneShot.getNow();
					transmiting.time =  sent + RTT;
					call Queue.enqueue(transmiting);

					call transSender.send(MSG,transmiting.dest);
				}
			}

			//else if(transmiting.fd->last_ack > transmiting.fd->last_send )

			else { //wrap

				if(transmiting.SeqNum <= transmiting.fd->last_ack  && transmiting.SeqNum > transmiting.fd->last_send){
						//put in pool
						//dbg(TRANSPORT_CHANNEL, "								ACKED 2\n ");
				}
				else{

					//dbg(TRANSPORT_CHANNEL, "Norm\n ");
					//dbg(TRANSPORT_CHANNEL, "NOT ACKED 2\n ");
					sent = call oneShot.getNow();
					transmiting.time =  sent + RTT;
					//transmiting.fd =
					call Queue.enqueue(transmiting);

					call transSender.send(MSG,transmiting.dest);


				}
			}
		}

		if(!call Queue.empty()){
			transmiting = call Queue.head();

			//if(transmiting.time < call oneShot.getNow()){
				//call oneShot.startOneShot(500);
			//}

			//else{

				dt = transmiting.time -  call oneShot.getNow();

				call oneShot.startOneShot(dt);

			//}
		}

		//dbg(TRANSPORT_CHANNEL, "size %hhu    SENT TO  %hhu    Flag %hhu    SeqNum %hhu     time  %hhu    \n", call Queue.size(),transmiting.dest,transmiting.type ,transmiting.SeqNum,transmiting.time);

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
		TCP->AdvertisedWindow = BUFFER_SIZE;
		TCP->SrcPort = fd->Src_port;//random asumm 200
		TCP->DstPort = addr->source_Port; //123

		fd->last_ack   = TCP->SeqNum;
		fd->last_send  = TCP->SeqNum;
		fd->last_write = TCP->SeqNum;

		//		//200				//123					//1
		KEY = fd->Src_port << 24 | addr->source_Port << 16 | addr->source_Addr;

		//dbg(TRANSPORT_CHANNEL, "CONNECTION :  l_syn %hhu   \n", TCP->SeqNum);

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
			dbg(TRANSPORT_CHANNEL, "work done...FIN 1...\n");
			//dbg(TRANSPORT_CHANNEL, "SRcport %hhu  destport %hhu destaddr %hhu\n",fd->Src_port, fd->Dest_port, fd->Dest_addr);
			TCP = (Transfer*) MSG.payload;
			TCP->Flag = FIN;
			fd->state = FIN_WAIT;
			//dbg(TRANSPORT_CHANNEL, "													SRcport %hhu  \n",TCP->Acknowledge);

			TCP->SeqNum = TCP->Acknowledge;

			retranmission(TCP->Flag, fd->Dest_addr, TCP->SeqNum , MSG ,fd);


    	}

		if(fd->state == CLOSED_WAIT){//server close
			uint32_t key;
			dbg(TRANSPORT_CHANNEL, "work done...FIN2...\n");
			TCP = (Transfer*) MSG.payload;
			TCP->Flag = FIN ;
			fd->state = LAST_ACK;
			TCP->SeqNum = TCP->Acknowledge ;
			TCP->Acknowledge = TCP->SeqNum +1;

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

	}

	uint8_t min(uint8_t EF,uint8_t PAYLOAD){

		uint8_t c = 0;
		 while ( EF &&  PAYLOAD ){

      		EF--;  PAYLOAD--; c++;

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

		temp = rand()% 126 + 129;

		for (i = 0 ; i < 255; i++){// use to fill up neighborList w/place holder value

			if(ports[i]== SENTINEL){
				continue;
			}

			if(temp == ports[i] || ports[i] == 80){
				temp = rand()% 254 + 129;
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
