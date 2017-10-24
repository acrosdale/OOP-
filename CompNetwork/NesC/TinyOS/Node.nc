/*
 * ANDES Lab - University of California, Merced
 * This class provides the basic functions of a network node.
 *
 * @author UCM ANDES Lab
 * @date   2013/09/03
 *
 */
#include <Timer.h>
#include "includes/command.h"
#include "includes/packet.h"
#include "includes/CommandMsg.h"
#include "includes/sendInfo.h"
#include "includes/channels.h"
#include "../../includes/Reliable_Transport.h"
#include "../../includes/distance_vector_routing.h"
#include "../../includes/socket.h"
#include <string.h>



#define CONNECT_WAIT 10000


#define  CLIENT  1

#define BEACON_PERIOD 1000 //write

#define BEACON 1000 //read


module Node{
   uses interface Boot;

   uses interface SplitControl as AMControl;
   uses interface Receive;
   uses interface SimpleSend as Sender;

  // Flooding Portion
   uses interface SimpleSend as FloodSender;
   uses interface Receive as FloodReceive;
   uses interface Receive as FloodReplyReceive;

   //Neighbor Discovery portion
    uses interface NeighborDiscovery;

   // Routing portion
    uses interface Routing;
    uses interface SimpleSend as RouteSender;
    uses interface Receive as RouteReceived;
    uses interface Receive as RouteReplyReceive;

     // Transport portion
    uses interface Transport;
    uses interface SimpleSend as TransSender;
    uses interface Receive as TransReceived;
    uses interface Receive as TransReplyReceive;

    //uses interface Timer<TMilli> as STimer;
    //uses interface Timer<TMilli> as CTimer;

    uses interface Timer<TMilli> as SP_Timer;
    uses interface Timer<TMilli> as CP_Timer;


    uses interface Hashmap<socket_t*> as Hash;

    uses interface Pool<socket_addr_t> as addr_pool;

    uses interface Queue<conn> as QueueS;

    uses interface Queue<conn> as QueueC;


    uses interface CommandHandler;

}

implementation{
   pack sendPackage;

   socket_t * fdC;
   socket_t * fdS;
   socket_addr_t * address;//Client
   socket_addr_t * addrs;//server


   char * names [] = {"ajames", "acrosdale", "abeltran2", "acerpa","ghostX)"};

   uint8_t namearr []={99,99,99,99,99};


  // uint8_t  read_buff[BUFFER_SIZE];
   //uint16_t bufflen;


   uint8_t write_counter = 0;
   bool write_off=TRUE;

   uint8_t over_read =0;


   uint8_t C_over_read =0;
   bool C_start = FALSE;

   uint8_t option=0;


   uint8_t leng=0;

   uint8_t SENTINEL = 999;
   uint8_t TRANSFER =0;

   bool message_space = FALSE;


   bool CLIENT_SIDE= FALSE;
   bool SERVER_SIDE= FALSE;

   bool active = FALSE;
   bool ONLY_ONCE=FALSE;

   uint8_t  tempbuff[BUFFER_SIZE];// temp buff
   uint8_t  BUFF[BUFFER_SIZE];

    uint8_t  S_BUFF[BUFFER_SIZE];
    uint8_t S_write_counter = 0;
    bool getscount = FALSE;
    bool reset_listen = FALSE;


   uint8_t use =0;
   uint8_t *AppBuff= NULL;
   uint8_t Destport;

   uint8_t  over_APP_buff[BUFFER_SIZE];
    uint8_t appcount =1,checker =0;


   void filler ();
   void clear_buff();

   void SetPayload(uint8_t *payload){

       AppBuff = payload;

   }

   uint8_t * getPayload(){

        return AppBuff;

   }


    void Setreciever(uint8_t dest){

          Destport = dest;

    }

      uint8_t getreciever(){

           return Destport;

    }



    void Setrecieverbuff(uint8_t APP[], uint8_t len, uint8_t len2)
    {
        uint8_t i =0,c, d, e,f, text_length, pattern_length, position = 0, diff =0;

        if(ONLY_ONCE == TRUE){


            text_length = appcount;
     	    pattern_length = len2 -len;
            i= len;

            /*while(i < len2){
                if(APP[i%BUFFER_SIZE] > 0 && APP[i%BUFFER_SIZE] < 128){
                    dbg(GENERAL_CHANNEL, "pat %c \n",APP[i%BUFFER_SIZE]);
                }


              i++;

           }*/


    	    if (pattern_length > text_length) {
    	        checker = 0;
    	    }

    	    for (c = 0; c < text_length - pattern_length; c++) {
    	        position = e = c;

    	        for (d = len; d < len2; d++) {
    	            if (APP[d] == over_APP_buff[e]) {
                        //dbg(GENERAL_CHANNEL, "found %c \n" ,over_APP_buff[e]);
    	                e++;
    	            }
    				else {
    	                break;
    	            }
    	        }
    	        if (d == pattern_length) {
    	            checker =position;
                    //dbg(GENERAL_CHANNEL, "found@ %hhu\n" , checker);
    	        }
            }


            if(checker == 0){
                i= appcount;
                f = appcount +12;
                //diff = len2 -len;
                while(i < f ){

                    //over_APP_buff[i%BUFFER_SIZE] = APP[i%BUFFER_SIZE];
                    //dbg(GENERAL_CHANNEL, "Booted@2 %c\n", APP[i%BUFFER_SIZE]);
                    if(APP[i%BUFFER_SIZE] > 0 && APP[i%BUFFER_SIZE] < 128){

                        dbg(GENERAL_CHANNEL, "data %c\n", APP[i%BUFFER_SIZE]);
                        over_APP_buff[i%BUFFER_SIZE] = APP[i%BUFFER_SIZE];

                        appcount++;
                    }
                    i++;

                    //diff--;
                    ONLY_ONCE == FALSE ;
                }

            }

        }

        if(ONLY_ONCE == FALSE && len < len2){
            i=len;


            for(i=len; i < len2 ;i++){


                over_APP_buff[i%BUFFER_SIZE] = APP[i%BUFFER_SIZE];
                //dbg(GENERAL_CHANNEL, "Booted@ %c\n", APP[i%BUFFER_SIZE]);

                appcount++;
            }

            ONLY_ONCE = TRUE;
        }




    }

    uint8_t getrecieverbuff(){

        uint8_t u=0;
        /*for(u=0; u < 128 ;u++){

            //over_APP_buff[i%BUFFER_SIZE] = APP[i%BUFFER_SIZE];
            if( over_APP_buff[u%BUFFER_SIZE] > 0 && over_APP_buff[u%BUFFER_SIZE] < 128){
                dbg(GENERAL_CHANNEL, "Booted3 %c\n", over_APP_buff[u%BUFFER_SIZE]);

            }


        }*/
        ONLY_ONCE = FALSE;

        //return Destport;

    }







    uint8_t GetName(){


        uint8_t temp = 0;
		uint16_t i = 0;

		temp = rand()% 5;

		for (i = 0 ; i < 5; i++){// use to fill up neighborList w/place holder value

			if(namearr[i]== SENTINEL){
				continue;
			}

			if(temp == namearr[i] ){
				temp = rand()% 255;
				i=0;//recheck
			}


		}

		namearr[use++]= temp;

		return temp;

    }

   // Prototypes
   void makePack(pack *Package, uint16_t src, uint16_t dest, uint16_t TTL, uint16_t Protocol, uint16_t seq, uint8_t *payload, uint8_t length);

   event void Boot.booted(){
      call AMControl.start();
      dbg(GENERAL_CHANNEL, "Booted\n");
   }

   event void AMControl.startDone(error_t err){
      if(err == SUCCESS){
         dbg(GENERAL_CHANNEL, "Radio On\n");

        call NeighborDiscovery.start();

        call Routing.start();
      }
      else{
         //Retry until successful
         call AMControl.start();
      }
   }

   event void AMControl.stopDone(error_t err){}


   event message_t* FloodReceive.receive(message_t* msg, void* payload, uint8_t len){
       if(len==sizeof(pack)){
         // pack* myMsg=(pack*) payload;
          dbg(FLOODING_CHANNEL, "FINAL DEST REACHED!! :^) \n");

       }
   	   return msg;
   }

  event message_t* FloodReplyReceive.receive(message_t* msg, void* payload, uint8_t len){
      if(len==sizeof(pack)){
         //pack* myMsg=(pack*) payload;
         dbg(GENERAL_CHANNEL, "REPLY DEST REACHED!! x^)\n");
      }
   	  return msg;
   }

//////8*********************************************/
   event message_t* RouteReceived.receive(message_t* msg, void* payload, uint8_t len){
       if(len==sizeof(pack)){
         // pack* myMsg=(pack*) payload;
          dbg(FLOODING_CHANNEL, "FINAL DEST REACHED!! :^) \n");

       }
   	   return msg;
   }

  event message_t* RouteReplyReceive.receive(message_t* msg, void* payload, uint8_t len){
      if(len==sizeof(pack)){
         //pack* myMsg=(pack*) payload;
         dbg(GENERAL_CHANNEL, "REPLY DEST REACHED!! x^)\n");
      }
   	  return msg;
   }
/////8*********************************************/
//_------------------------------------------------

 event message_t* TransReceived.receive(message_t* msg, void* payload, uint8_t len){
    if(len==sizeof(pack)){
      // pack* myMsg=(pack*) payload;
      // dbg(FLOODING_CHANNEL, "FINAL DEST REACHED!! :^) \n");

    }
    return msg;
 }

 event message_t* TransReplyReceive.receive(message_t* msg, void* payload, uint8_t len){
   if(len==sizeof(pack)){
      //pack* myMsg=(pack*) payload;
      //dbg(GENERAL_CHANNEL, "REPLY DEST REACHED!! x^)\n");
   }
   return msg;
 }

//-------------------------------------------------



   event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){
      dbg(GENERAL_CHANNEL, "Packet Received\n");
      if(len==sizeof(pack)){
         pack* myMsg=(pack*) payload;
         dbg(GENERAL_CHANNEL, "Package Payload: %s\n", myMsg->payload);
         return msg;
      }
      dbg(GENERAL_CHANNEL, "Unknown Packet Type %d\n", len);
      return msg;
   }


   event void CommandHandler.ping(uint16_t destination, uint8_t *payload){
      dbg(GENERAL_CHANNEL, "PING EVENT \n");

      makePack(&sendPackage, TOS_NODE_ID, destination, 0, 0, 0, payload, PACKET_MAX_PAYLOAD_SIZE);

       call RouteSender.send(sendPackage, destination);
   }


   event void CommandHandler.printNeighbors(){
       //printNeighbors
        call NeighborDiscovery.print();
        //call Routing.start();
   }

   event void CommandHandler.printRouteTable(){
       call Routing.print();

   }

   event void CommandHandler.printLinkState(){}

   event void CommandHandler.printDistanceVector(){

   }

   /*k=temp.start;
   while(k < temp.finish-temp.start){

       dbg(TRANSPORT_CHANNEL, "Reading## %c\n",temp.APP_buff[k%BUFFER_SIZE]);
       k++;
   }*/


//-----------------------------------client function------------------------

    event void CP_Timer.fired(){

        uint8_t i = 0, j=0,z=0,k=0, Queue_size = call QueueC.size();
        uint16_t a;
        uint8_t readcursor =0;
        uint8_t c, d, e, text_length, pattern_length, position = 0;
        conn temp;
        //uint8_t write_counter[Queue_size];

        //while(i < Queue_size){
            //dbg(TRANSPORT_CHANNEL, "CLieNT SENDING\n");
            temp = call QueueC.dequeue();

            if( write_off == FALSE){
                for( z = 0; z < TO_WRITE;){

                    BUFF[z]   = tempbuff[write_counter];

                    if( (char)tempbuff[write_counter] == '\n'){
                        write_off=TRUE;
                        break;
                    }
                    write_counter++;
                    z++;
                }
                call Transport.write(temp.fd, BUFF,TO_WRITE);
            }

            if(write_off == TRUE){ // read info if their is any
                    call CP_Timer.startPeriodic(2*BEACON_PERIOD);


                call Transport.read(temp.fd, temp.read_buff, TO_WRITE);

                while(j < 12 ){

                    if(C_start == FALSE){
                        dbg(TRANSPORT_CHANNEL, "START>>>\n");
                        temp.start = C_over_read%BUFFER_SIZE;
                        C_start = TRUE;
                    }

                    a = temp.read_buff[j];
                    if(a!=0){
                        //dbg(TRANSPORT_CHANNEL, "Reading## %c\n",a);
                    }


                    if(a > 0 && a<= 127 ){
                        //temp.APP_buff[C_over_read%BUFFER_SIZE] = a;

                        temp.APP_buff[C_over_read%BUFFER_SIZE]= a;
                    }

                    if((char)a == '\n'){
                        //dbg(TRANSPORT_CHANNEL, "Reading>>>\n");
                        temp.finish = C_over_read%BUFFER_SIZE;


                        call Transport.print(temp.APP_buff,temp.start, temp.finish-temp.start, TRUE);
                        //reset
                        //temp.start = temp.finish =0;
                        j=0;
                        while(j < 128){ temp.APP_buff[j] =0; temp.read_buff[j] =0;j++; } //clean the APP buffer

                        C_start = FALSE;
                        j=0;
                        C_over_read=0;;
                        break;
                    }

                    //dbg(TRANSPORT_CHANNEL, "Reading## %hhu\n",temp.APP_buff[over_read%BUFFER_SIZE]);
                    j++;
                    C_over_read++;

                }

            }

            call QueueC.enqueue(temp);
        //    i++;
        //}

    }

    event void * Transport.connectDone(socket_t * FD){

        conn  socket;

        socket.fd = FD;
        call QueueC.enqueue(socket);
        //buff = getPayload();

        memcpy(tempbuff, (getPayload()), 24 *sizeof(getPayload()));

        dbg(TRANSPORT_CHANNEL, "CLIENT: Connection DONE %d %d %d\n", FD->Src_port, FD->Dest_port , FD->Dest_addr);


        //start timer. periodic
        call CP_Timer.startPeriodic(BEACON_PERIOD);

        //call Transport.close(FD);

    }




//--------------------------SERVER FUNTION---------------------------------


    event void SP_Timer.fired(){

        uint16_t i=0,j,j1,j2,z,k,k2,read;
        uint16_t a,b,c,d,e ,f;
        uint8_t track =0;
        conn temp;

        while(i < call QueueS.size()){
            j=0,j1=0,j2=0,z,k=0,k2=0;

            //dbg(TRANSPORT_CHANNEL, "Reading#zzzz \n");


            temp = call QueueS.dequeue();

            read =  call Transport.read(temp.fd, temp.read_buff, TO_WRITE);

            //call Transport.print(temp.read_buff,read, TRUE);

            while(j < read){

                a=(temp.read_buff[j]);
                //dbg(TRANSPORT_CHANNEL, "Reading# %hhu \n", a);

                if(a == 'h' ){

                    b=  temp.read_buff[j+1] ;

                    if(b == 'e'){

                        c=temp.read_buff[j+2];

                        if(c== 'l'){

                            d=temp.read_buff[j+3] ;
                            if(d =='l'){
                                //dbg(TRANSPORT_CHANNEL, "Reading# %c \n", a);
                                over_read=0;

                                temp.start = over_read;  // start a read counter
                                //dbg(TRANSPORT_CHANNEL, "Reading@# %hhu \n", over_read);
                                option = 1; //take in hello comand message
                                Setreciever(temp.fd->Dest_port);
                                while(j1 < 128){ temp.APP_buff[(j1)%BUFFER_SIZE] =0;  S_BUFF[j1]=0;j1++; } //clean the APP buffer
                                j1=0;
                            }
                        }

                    }

                }

            if(a == 'm' ){

                b=  temp.read_buff[j+1] ;

                if(b == 'e'){

                    c=temp.read_buff[j+2];

                    if(c== 's'){

                        d=temp.read_buff[j+3] ;
                        if(d =='s'){

                            over_read=0;

                            temp.start = over_read;


                            option = 3; //take in message c
                            while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer
                            j1=0;
                        }
                    }

                }

            }


                if(a == 'l' ){

                    b=  temp.read_buff[j+1] ;

                    if(b == 'i'){

                        c=temp.read_buff[j+2];

                        if(c== 's'){

                            d=temp.read_buff[j+3] ;
                            if(d =='t'){
                                //dbg(TRANSPORT_CHANNEL, "Reading# %c \n", a);
                                over_read=0;
                                temp.start = over_read;  // start a read counter
                                //dbg(TRANSPORT_CHANNEL, "Reading@# %hhu \n", over_read);
                                option = 5; //take in hello comand message
                                Setreciever(temp.fd->Dest_port);
                                while(j1 < 128){ temp.APP_buff[(j1)%BUFFER_SIZE] =0;  j1++; } //clean the APP buffer
                                j1=0;
                            }
                        }

                    }

                }

                if(option ==1 ){  // get name and port  for a respose
                    //bool Nme= FALSE;
                    //uint8_t inter_buff[128];

                    while( j1 < 12){

                        a=(temp.read_buff[j1]);

                        if((char)a == '\r'){

                            uint8_t tn = over_read;
                            uint8_t tntrack =0;
                            uint8_t o =0;
                            //dbg(TRANSPORT_CHANNEL, "IN\n");
                            while(tn > 0){
                                if(temp.APP_buff[tn%BUFFER_SIZE] ==' ' ){
                                    tntrack = tn;
                                    break;
                                }
                                tn--;
                            }
                            tn = over_read;
                            while(tntrack < tn){
                                if(temp.APP_buff[tntrack%BUFFER_SIZE] ==0){

                                }
                                else{
                                    temp.name[o] = temp.APP_buff[tntrack%BUFFER_SIZE];
                                    //dbg(TRANSPORT_CHANNEL, "APP%c \n",temp.name[o]);
                                }

                                tntrack++;
                                o++;
                            }
                            temp.name[o] ='/';
                        }
                        temp.APP_buff[over_read%BUFFER_SIZE] = a;//add everthing arround it hello name port
                        //dbg(TRANSPORT_CHANNEL, "App buff in %hhu\n",temp.APP_buff[over_read%BUFFER_SIZE]);
                        j1++;
                        over_read++;

                        if(a =='\n' ){//once end char detected end get name option got to option reply.
                            option=2;
                            temp.finish = (over_read%BUFFER_SIZE);

                            break;
                        }
                    }
                }

                if(option ==3 ){// get meessage response
                    dbg(TRANSPORT_CHANNEL, "MESAGE \n");
                    while( j2 < 12){

                        a = temp.read_buff[j2]; //get msg from read buff
                        //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
                        if((char)a == ' ' && message_space == FALSE){//when the first ' ' is detect after the word message add the sender name
                            track =0;
                            temp.APP_buff[over_read%BUFFER_SIZE] = a;
                            over_read++;
                            //k=0;

                            while(k < 20){// retrive the sender anme from socket
                                //dbg(TRANSPORT_CHANNEL, "YYYYYYYz %c\n",temp.name[k]);
                                if(temp.name[k] == 0){
                                    track++;
                                    //over_read++;
                                    k++;
                                }
                                if(track == 3 || temp.name[k] =='/'){
                                    break;
                                }
                                if(temp.name[k] != 0){
                                    temp.APP_buff[over_read%BUFFER_SIZE]= temp.name[k];
                                    //dbg(TRANSPORT_CHANNEL, "Reading@@ %c\n",temp.name[k]);
                                    over_read++;
                                    k++;
                                }
                            }

                            message_space = TRUE;
                        }
                        temp.APP_buff[over_read%BUFFER_SIZE] = a;
                        //dbg(TRANSPORT_CHANNEL, "Reading## %c\n",a);
                        j2++;
                        over_read++;

                        if(a =='\n' ){
                            temp.finish = (over_read%BUFFER_SIZE)-1;
                            option=4;

                            break;
                        }
                    }
                }

                over_read++;
                j+=12;
            }


            //dbg(TRANSPORT_CHANNEL, "         enter opt %hhu \n",temp.fd->active);
            if(option == 2 && temp.fd->Dest_port == getreciever() ){ //if any has anything to write do it

                //dbg(TRANSPORT_CHANNEL, "         enter opt %hhu \n",option);
                  for( z = 0; z < TO_WRITE;){

                    if(temp.start < temp.finish){

                        if(temp.APP_buff[temp.start%BUFFER_SIZE] == 0){//skip over junk char
                        //    dbg(TRANSPORT_CHANNEL, "                        op done 2\n");
                            temp.start++;
                            //continue;
                        }

                        S_BUFF[z]   = temp.APP_buff[temp.start%BUFFER_SIZE];
                        //dbg(TRANSPORT_CHANNEL, "         enter opt %c \n",S_BUFF[z] );

                        temp.start++;

                        if( (char)temp.APP_buff[temp.start%BUFFER_SIZE] == '\n'){

                            S_BUFF[z]   = temp.APP_buff[temp.start%BUFFER_SIZE];
                            //S_BUFF[z+1] = temp.APP_buff[temp.start%BUFFER_SIZE] >> 8;


                            //dbg(TRANSPORT_CHANNEL, "                       OP done \n");
                            //temp.fd->active == FALSE;
                            option =0; break;


                        }

                    }

                    z++;
                }
                //dbg(TRANSPORT_CHANNEL, "                       OP done  %hhu\n",temp.fd->Dest_addr );
                call Transport.write(temp.fd, S_BUFF,TO_WRITE);

            }


            if (option == 4) { //send msg to ever one
                conn all;
                uint16_t y = 0,p = 0, q=5;
                over_read = 0;

            //    dbg(TRANSPORT_CHANNEL, "MESAGE \n");

                while (p < 5) {
                    for (z = 0; z < TO_WRITE;) { //who can write write

                        if (temp.start < temp.finish) {

                            if (temp.APP_buff[temp.start % BUFFER_SIZE] == 0) { //skip junk char
                                //dbg(TRANSPORT_CHANNEL, "                        op done 2\n");

                            } else {


                                S_BUFF[z] = temp.APP_buff[temp.start % BUFFER_SIZE];
                                //dbg(TRANSPORT_CHANNEL, "app %c \n", temp.APP_buff[temp.start % BUFFER_SIZE]);

                                if ((char) temp.APP_buff[temp.start % BUFFER_SIZE] == '\n') {

                                    //dbg(TRANSPORT_CHANNEL, "                         OP done %hhu \n", temp.fd-> Dest_port);
                                    y = 0;
                                    call Transport.write(temp.fd, S_BUFF, TO_WRITE);

                                    while (y < call QueueS.size() && call QueueS.size() > 0) { //write to every on else in QueueS

                                        all = call QueueS.element(y);
                                        //dbg(TRANSPORT_CHANNEL, "                       OPing %hhu \n", all.fd->Dest_port);
                                        call Transport.write(all.fd, S_BUFF, TO_WRITE);

                                        y++;
                                    }
                                    option = 0;
                                    break;

                                }

                            }

                            temp.start++;
                        }

                        z++;
                    }
                    y = 0;
                    call Transport.write(temp.fd, S_BUFF, TO_WRITE);

                    while (y < call QueueS.size() && call QueueS.size() > 0) { //write to every on else in QueueS

                        all = call QueueS.element(y);
                        dbg(TRANSPORT_CHANNEL, "                       SEND TO %hhu \n", all.fd->Dest_port);
                        call Transport.write(all.fd, S_BUFF, TO_WRITE);

                        y++;
                    }
                    p++;

                }
            }


            if (option == 5) { //list user
                uint16_t l = 0, cp = 0;
                conn user;
                char myMsg[11] = "listusers\r\n";
                c = 0;
                j1 = 0;


                //while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer

                j1 = temp.start;
                track = 0;
                while (j1 < 11) {

                    if (myMsg[j1] == '\r') { //get all name from register socket

                        temp.APP_buff[(j1) % BUFFER_SIZE] = ' ';
                        j++;

                        while (l < 20) {

                            if (temp.name[l] == 0) {
                                track++;
                            }
                            if (track == 3 ||  temp.name[l] == '/') {
                                break;
                            }

                            temp.APP_buff[(j1 + l) % BUFFER_SIZE] = temp.name[l];
                            //dbg(TRANSPORT_CHANNEL, "                       OPing000 %c \n", temp.name[l]);
                            l++;
                        }
                        temp.APP_buff[(j1 + l) % BUFFER_SIZE] = ',';

                        cp = (j1 + l) + 1;
                        l = 0;

                        while (l < call QueueS.size()) { //write to every on else in QueueS
                            track = 0;
                            c = 0;
                            user = call QueueS.element(l);
                            while (c < 20) {
                                if (user.name[c] > 0 && user.name[c] <128) {
                                    break;

                                }
                            }

                            while (c < 20) {

                                if (user.name[c] == 0) {
                                    track++;
                                    c++;
                                }
                                if (track == 5 || user.name[c] == '/') {
                                    break;
                                }
                                if (user.name[c] != 0 ) {
                                    //dbg(TRANSPORT_CHANNEL, "                       OPing2 %c \n", user.name[c]);
                                    temp.APP_buff[(cp + c) % BUFFER_SIZE] = user.name[c];
                                    c++;
                                }
                            }

                            temp.APP_buff[(cp + c) % BUFFER_SIZE] = ',';
                            cp = (cp + c) + 1;
                            l++;
                        }
                        l = cp;
                    }

                    temp.APP_buff[(j1 + l) % BUFFER_SIZE] = myMsg[j1];
                    j1++;
                }

                temp.finish = (j1 + l) % BUFFER_SIZE;

                /*j1 = temp.start;
                while (j1 < temp.finish) {

                    dbg(TRANSPORT_CHANNEL, "     APP BUFF %c \n", temp.APP_buff[(j1) % BUFFER_SIZE]);

                    j1++;
                }*/
                option = 6;
            }

            if (option == 6 && temp.fd->Dest_port == getreciever() ) {
                conn all;
                uint16_t y = 0;

                //uint8_t inter_buff[BUFFER_SIZE];
                over_read = 0;

                clear_buff();
                for (z = 0; z < TO_WRITE;z++) {

                    if (temp.start < temp.finish) {

                        if (temp.APP_buff[temp.start % BUFFER_SIZE] == 0) {
                            //dbg(TRANSPORT_CHANNEL, "                        op done %hhu\n", temp.start);
                            //temp.start+=1;
                            //dbg(TRANSPORT_CHANNEL, "                        op done %hhu\n", temp.start);
                            //continue;
                        }
                        if(temp.APP_buff[temp.start % BUFFER_SIZE] != 0){


                            S_BUFF[z] = temp.APP_buff[temp.start % BUFFER_SIZE] ;
                            //dbg(TRANSPORT_CHANNEL, "app %c \n", S_BUFF[z]);

                            if ((char) temp.APP_buff[temp.start % BUFFER_SIZE] == '\n') {
                                //dbg(TRANSPORT_CHANNEL, "                         OP done %hhu \n",temp.fd->Dest_port);
                                //call Transport.write(temp.fd, S_BUFF,TO_WRITE);
                                option = 0;
                                break;
                            }
                        }

                        temp.start++;
                    }

                }

                z=0;
                while(z < 12){
                    //if(inter_buff[z] != 0){
                        //S_BUFF[z] = inter_buff[z];
                        //dbg(TRANSPORT_CHANNEL, "APP DATz %c\n", S_BUFF[z] );
                    //}
                    z++;
                }
                call Transport.write(temp.fd, S_BUFF,TO_WRITE); //write to the one in use

            }



            call QueueS.enqueue(temp);
            i++;
        }

    }


    event void * Transport.accept( socket_t * FD){

        conn  socket;
        //socket.name = GetName();
        socket.fd = FD;
        socket.fd->active = FALSE;
        //socket.start =254;
        call QueueS.enqueue(socket);

        dbg(TRANSPORT_CHANNEL, "SERVER: Hello %d %d %d\n", FD->Src_port, FD->Dest_port , FD->Dest_addr);

        call SP_Timer.startPeriodic(BEACON);

        //call Transport.close(FD);
    }


   //---------------------------------------------------------------------------



   event void CommandHandler.setTestServer(){


       fdS = call Transport.socket(); // create socket with the socket();

       if(fdS != NULL){
           dbg(GENERAL_CHANNEL, "socket created... starting addr alloc\n");

       		if(!call addr_pool.empty()){

                addrs = call addr_pool.get();

                addrs->source_Addr = SERVER_ADDR;//1 SERVER_PORT
                addrs->source_Port = SERVER_PORT;//123
                dbg(GENERAL_CHANNEL, "addr allocated... begin binding ()\n");

                call Transport.bind(fdS, addrs);
                dbg(GENERAL_CHANNEL, "bind complete().....starting listen\n");

                call Transport.listen(fdS);
                dbg(GENERAL_CHANNEL, "listen.....\n");

           }
       }
   }


   event void CommandHandler.setTestClient(){

        fdC = call Transport.socket(); // create socket with the socket();


       if(fdC != NULL){
           dbg(GENERAL_CHANNEL, "socket created... starting addr alloc\n");

           if(!call addr_pool.empty()){

               address = call addr_pool.get();
               address->source_Addr = TOS_NODE_ID;

               call Transport.bind(fdC, address);
               dbg(GENERAL_CHANNEL, "bind complete()......\n");

               dbg(GENERAL_CHANNEL, "addr allocated... begin connect()\n");

               if(!call addr_pool.empty()){

                   socket_addr_t * add = call addr_pool.get();//server
                   add->source_Addr = SERVER_ADDR;
                   add->source_Port = SERVER_PORT;


                   if(call Transport.connect(fdC, add) == SUCCESS)
                   {
                       dbg(GENERAL_CHANNEL, "Connecting>>>>>\n");
                   }

                }
            }
       }
   }


   event void CommandHandler.setAppServer(){


   }

   event void CommandHandler.setAppClient(uint8_t *payload){

        uint8_t i =0;

        if(payload[0] == 'h' || payload[0] == 'H'){

            SetPayload(payload);

            signal CommandHandler.setTestClient();
            active = TRUE;
            write_off = FALSE;
        }

        if(active == TRUE && call QueueC.size() >= 1){


            if(payload[0] == 'm' || payload[0] == 'M'){

                SetPayload(payload);
                while(payload[i] != '\n'){
                    i++;
                }
                write_counter =0;
                memcpy(tempbuff, (getPayload()), (i+1)* sizeof(getPayload()));
                write_off = FALSE;
                call CP_Timer.startPeriodic(BEACON_PERIOD);
            }

            if(payload[0] == 'l' || payload[0] == 'L'){
                SetPayload(payload);
                while(payload[i] != '\n'){
                    //dbg(GENERAL_CHANNEL, "L START %c\n",payload[i]);
                    i++;
                }
                write_counter =0;
                filler ();
                memcpy(tempbuff, (getPayload()), (i+1)* sizeof(getPayload()));
                write_off = FALSE;
                call CP_Timer.startPeriodic(BEACON_PERIOD);

            }

        }

   }

   void makePack(pack *Package, uint16_t src, uint16_t dest, uint16_t TTL, uint16_t protocol, uint16_t seq, uint8_t* payload, uint8_t length){
      Package->src = src;
      Package->dest = dest;
      Package->TTL = TTL;
      Package->seq = seq;
      Package->protocol = protocol;
      //payload = (uint8_t*) routingTable;
      memcpy(Package->payload, Transit, MAX_PAYLOAD);
   }

   void filler (){ // use to fill the neighborList with the known NULL value place holder
       uint16_t i = 0;

       for (i = 0 ; i < 128; i++){// use to fill up neighborList w/place holder value

           tempbuff[i] = 0;
           BUFF[i]=0;

       }

   }

   void clear_buff(){ // use to fill the neighborList with the known NULL value place holder
       uint16_t i = 0;

       for (i = 0 ; i < 128; i++){// use to fill up neighborList w/place holder value

           S_BUFF[i] = 0;

       }

   }
}



//dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
/*

a=(temp.read_buff[j1+1]<< 8 ) | (temp.read_buff[j1] & 0xff);
dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
if((char)a == '\r'){
    temp.APP_buff[over_read%BUFFER_SIZE] = ' ';
    over_read++;

    //dbg(TRANSPORT_CHANNEL, "size %hhu\n",call QueueS.size() );
    while(l < call QueueS.size()){//write to every on else in QueueS

        user = call QueueS.element(l);
        k=0;
        track =0;
        while(k < 20){
            if(user.name[k] == 0){
                track++;
                //over_read++;
                k++;
            }
            if(track == 3){
                //dbg(TRANSPORT_CHANNEL, "break\n");
                break;
            }

            if(user.name[k] != 0){
                temp.APP_buff[over_read%BUFFER_SIZE]= user.name[k];
                //dbg(TRANSPORT_CHANNEL, "Reading@@ %c\n",temp.APP_buff[over_read%BUFFER_SIZE]);
                over_read++;
                k++;
            }
        }
        temp.APP_buff[over_read%BUFFER_SIZE] = ' ';
        over_read++;

        l++;

    }
    k=0;
    track =0;
    while(k < 20){
        if(temp.name[k] == 0){
            track++;
            //over_read++;
            k++;
        }

        if(track == 3){
            //dbg(TRANSPORT_CHANNEL, "break\n");
            break;
        }

        if(temp.name[k] != 0){
            temp.APP_buff[over_read%BUFFER_SIZE]= temp.name[k];
            //dbg(TRANSPORT_CHANNEL, "Reading@@2 %c\n",temp.name[k]);
            over_read++;
            k++;
        }
    }
}

//temp.APP_buff[over_read%BUFFER_SIZE] = a;
//dbg(TRANSPORT_CHANNEL, "App buff in %c\n",temp.APP_buff[over_read%BUFFER_SIZE]);
j1+=2;
over_read++;

if(a =='\n' ){
    option=0;
    temp.finish = (over_read%BUFFER_SIZE)-1;
    i=0;

    while(i < temp.finish){

        //dbg(TRANSPORT_CHANNEL, "done %c\n",temp.APP_buff[i%BUFFER_SIZE]);
        i++;
    }

    //dbg(TRANSPORT_CHANNEL, "Reading#Z \n");
    break;
}
}
*/



/*else if(a == 'm'){

    b=(temp.read_buff[j+3]<< 8 ) | (temp.read_buff[j+2] & 0xff);

    if(b == 'e'){

        c=(temp.read_buff[j+5]<< 8 ) | (temp.read_buff[j+4] & 0xff);

        if(c== 's'){

            d=(temp.read_buff[j+7]<< 8 ) | (temp.read_buff[j+6] & 0xff);
            if(d =='s'){
                over_read=0;

                temp.start = over_read;

                option = 3; //take in message c
                while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer
                j1=0;

            }
        }
    }

}
else if (a == 'l'){

    b=(temp.read_buff[j+3]<< 8 ) | (temp.read_buff[j+2] & 0xff);

    if(b == 'i'){

        c=(temp.read_buff[j+5]<< 8 ) | (temp.read_buff[j+4] & 0xff);

        if(c== 's'){

            d=(temp.read_buff[j+7]<< 8 ) | (temp.read_buff[j+6] & 0xff);
            if(d =='t'){
                over_read=0;
                temp.start = over_read;
                Setreciever(temp.fd->Dest_port);
                option = 5; //take in message c
                while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer
                j1=0;

            }
        }
    }

}

if(option ==1 ){  // get name and port  for a respose

    while( j1 < 12){

        a=(temp.read_buff[j1]);
        dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
        /*if((char)a == '\r'){
            //add the port number before the end
            temp.APP_buff[over_read%BUFFER_SIZE] = temp.fd->Dest_port;
            over_read++;

            while(k < 20){//from the APP buff extract name  after hello before port number
                if(temp.APP_buff[(temp.start+6+k)%BUFFER_SIZE] == 0){// junk character
                    k++;
                }

                if(temp.APP_buff[(temp.start+6+k)%BUFFER_SIZE] == temp.fd->Dest_port){
                    //break;
                    while( k2 < (20-k )){// fill the rest of namesapce with know junk

                        temp.name[k]=0;
                        //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",temp.name[k]);
                        k++;k2++;
                    }
                    break;
                }

                temp.name[k] = temp.APP_buff[(temp.start+6+k)%BUFFER_SIZE];//put the name into that socket name.
                //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",temp.name[k]);
                //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",temp.name[k]);
                k++;
            }
        }
        temp.APP_buff[over_read%BUFFER_SIZE] = a;//add everthing arround it hello name port
        //dbg(TRANSPORT_CHANNEL, "App buff in %c\n",temp.APP_buff[over_read%BUFFER_SIZE]);
        j1+=2;
        over_read++;

        if(a =='\n' ){//once end char detected end get name option got to option reply.
            option=2;
            temp.finish = (over_read%BUFFER_SIZE)-1;
            //dbg(TRANSPORT_CHANNEL, "Reading#Z \n");
            break;
        }
    }
}

if(option ==3 ){// get meessage response

    while( j2< 12){

        a=(temp.read_buff[j2+1]<< 8 ) | (temp.read_buff[j2] & 0xff); //get msg from read buff
        //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
        if((char)a == ' ' && message_space == FALSE){//when the first ' ' is detect after the word message add the sender name
            track =0;
            temp.APP_buff[over_read%BUFFER_SIZE] = a;
            over_read++;
            //k=0;

            while(k < 20){// retrive the sender anme from socket
                //dbg(TRANSPORT_CHANNEL, "YYYYYYYz %c\n",temp.name[k]);
                if(temp.name[k] == 0){
                    track++;
                    //over_read++;
                    k++;
                }
                if(track == 3){
                    break;
                }
                if(temp.name[k] != 0){
                    temp.APP_buff[over_read%BUFFER_SIZE]= temp.name[k];
                    //dbg(TRANSPORT_CHANNEL, "Reading@@ %c\n",temp.name[k]);
                    over_read++;
                    k++;
                }
            }

            message_space = TRUE;
        }
        temp.APP_buff[over_read%BUFFER_SIZE] = a;
        //dbg(TRANSPORT_CHANNEL, "Reading## %c\n",a);
        j2+=2;
        over_read++;

        if(a =='\n' ){
            option=4;
            temp.finish = (over_read%BUFFER_SIZE);
            k = temp.start;
            while(k < temp.finish){

                //dbg(TRANSPORT_CHANNEL, "MESS: %c \n", temp.APP_buff[k %BUFFER_SIZE]);
                k++;
            }

            break;
        }
    }
}

*/
