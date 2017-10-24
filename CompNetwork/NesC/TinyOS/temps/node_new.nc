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



#define CONNECT_WAIT 10000


#define  CLIENT  12

#define  WORK_DONE  130
#define  TO_WRITE   5




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

    uses interface Queue<socket_t*> as QueueS;

    uses interface Queue<socket_t*> as QueueC;


    uses interface CommandHandler;

}

implementation{
   pack sendPackage;

   socket_t * fdC;
   socket_t * fdS;
   socket_addr_t * address;//Client
   socket_addr_t * addrs;//server

   uint8_t  read_buff[BUFFER_SIZE];
   //uint16_t bufflen;
   JOB count[20];

   uint8_t write_counter = TO_WRITE;
   uint8_t over_count = 1;
   uint8_t over_read[CLIENT];
    uint8_t over_r;
   uint8_t leng=0;

   uint8_t SENTINEL = 999;
   uint8_t TRANSFER =0;
   bool once = FALSE;
   bool DONE = FALSE;
   bool CLIENT_SIDE= FALSE;
   bool SERVER_SIDE= FALSE;

   bool ONLY_ONCE=FALSE;
   uint8_t  buff[BUFFER_SIZE];
   void filler ();
    void reorder (){
        uint8_t  i;
        uint8_t  sen_count =0;// SENTINEL count
        uint8_t temp_read[CLIENT];

        for(i=0;i<CLIENT;i++){

            if(over_read[i] != SENTINEL){

                temp_read[i]= over_read[i];

            }
            else{
                sen_count++;
            }

        }

        for(i= sen_count;i<CLIENT;i++){


            temp_read[i] = SENTINEL;

        }

        for(i=0;i<CLIENT;i++){

            over_read[i] = temp_read[i];

        }

    }
   //global
   //Transport Table[len_of_buff];





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


//-----------------------------------client function------------------------

    event void CP_Timer.fired(){

        uint8_t i = 0, j=0,z=0,k=0, Queue_size = call QueueC.size();
        socket_t * temp= NULL;
        //uint8_t write_counter[Queue_size];




        while(i < Queue_size && !call QueueC.empty() && CLIENT_SIDE ==FALSE){
            //dbg(TRANSPORT_CHANNEL, "CLieNT SENDING %hhu\n", Queue_size  );


            for( z = 0; z < TO_WRITE; z++){
                //filler ();
                buff[z] =  over_count;
                over_count++;
                //dbg(TRANSPORT_CHANNEL, "send buff %hhu\n", buff[z]);

            }
            //dbg(TRANSPORT_CHANNEL, "over_count A%hhu\n", over_count);


            temp = call QueueC.dequeue();
            //write  server side
            leng = call Transport.write(temp, buff,TO_WRITE);


            if(over_count-1 == WORK_DONE){

                CLIENT_SIDE = TRUE;
            }
            else{
                call QueueC.enqueue(temp);
                //over_count = leng;
            }
            // keep track of socket_t
            i++;
        }


    }

    event void * Transport.connectDone(socket_t * FD){

        dbg(TRANSPORT_CHANNEL, "CLIENT: Connection DONE %d %d %d\n", FD->Src_port, FD->Dest_port , FD->Dest_addr);

        call QueueC.enqueue(FD);

        //start timer. periodic
        call CP_Timer.startPeriodic(BEACON_PERIOD);

        //dbg(TRANSPORT_CHANNEL, "CLIENT: Connection DONE\n");

        //call Transport.close(FD);

    }




//--------------------------SERVER FUNTION---------------------------------


    event void SP_Timer.fired(){

        uint16_t i =0,l=0, Queue_size = call QueueS.size();
        socket_t * temp= NULL;

        if(ONLY_ONCE == FALSE){
            filler();
            ONLY_ONCE =TRUE;
        }

        while(i < Queue_size  && !call QueueS.empty()){

            temp = call QueueC.dequeue();
            // read client side
            //dbg(GENERAL_CHANNEL, "READING\n");
            l=  call Transport.read(temp, read_buff, TO_WRITE);
            //dbg(TRANSPORT_CHANNEL, "server R  %hhu \n", l);
            over_r +=l;

            //call Transport.print(read_buff,l);


            if(over_r == WORK_DONE){

                //over_read = SENTINEL;
                //reorder();
                //dbg(GENERAL_CHANNEL, "          PORT %hhu  is done writing \n", temp->Dest_port);
                //call Transport.close(temp);

            }
            else{
                call QueueC.enqueue(temp);

            }

            i++;
        }

    }


    event void * Transport.accept( socket_t * FD){

        dbg(TRANSPORT_CHANNEL, "SERVER: Connection ACCEPTED %d %d %d\n", FD->Src_port, FD->Dest_port , FD->Dest_addr);
        call QueueS.enqueue(FD);

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


   event void CommandHandler.setAppServer(){}

   event void CommandHandler.setAppClient(){}

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

       for (i = 0 ; i < 12; i++){// use to fill up neighborList w/place holder value
           over_read[i]= SENTINEL;


       }

   }
}

/*  steps setTestClient

     create socket with the socket();
     connect the socket to the addr of the server using connect()
     send and recieve data using write and read call repectively

     global fd = socket(); socket address =  NODE_ID,[srcPort]/Only source info.
     bind(fd, socket address); server address = [dest],[destPort]// Only dest info.
     if you are able to connect(fd, server address)

     startTimer(CLIENT_WRITE_TIMER) global variable amount of data equal to [transfer]

     timer fired:
     if all data in buffer has been written or the buffer empty
     create new data for the buffer // data is from 0 to [transfer]
*/


/*  steps setTestServer

     create socket with the socket();
     bind the addr to a sockket using BIND()
     listen for connections with listen ()
     Accept a connection with Accept
     send and recieve data using write and read call repectively

     global fd = socket(); socket address =  NODE_ID,[port]//Only source info.
     bind(fd, socket address); startTimer(Attempt_Connection_Time);

     timer fired:
     //int newFd = accept(); if newFd not NULL_SOCKET
     add to list of accepted sockets
     for all sockets added
     read data and print

*/
