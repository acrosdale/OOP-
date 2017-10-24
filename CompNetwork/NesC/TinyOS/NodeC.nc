/**
 * ANDES Lab - University of California, Merced
 * This class provides the basic functions of a network node.
 *
 * @author UCM ANDES Lab
 * @date   2013/09/03
 *
 */

#include <Timer.h>
#include "includes/CommandMsg.h"
#include "includes/packet.h"
#include "../../includes/socket.h"



configuration NodeC{
}
implementation {
    components MainC;
    components Node;
    components new AMReceiverC(AM_PACK) as GeneralReceive;

    components new TimerMilliC() as STimer;
    components new TimerMilliC() as CTimer;


    components new TimerMilliC() as SP_Timer;
    components new TimerMilliC() as CP_Timer;

    //Node.STimer -> STimer;
    //Node.CTimer -> CTimer;

    Node.SP_Timer -> STimer;
    Node.CP_Timer -> CTimer;


    Node -> MainC.Boot;

    Node.Receive -> GeneralReceive;

    components ActiveMessageC;
    Node.AMControl -> ActiveMessageC;

    components new SimpleSendC(AM_PACK);
    Node.Sender -> SimpleSendC;

    components CommandHandlerC;
    Node.CommandHandler -> CommandHandlerC;



    components FloodingC;
    Node.FloodSender -> FloodingC.SimpleSend;
    Node.FloodReceive-> FloodingC.MainReceive;
    Node.FloodReplyReceive-> FloodingC.ReplyReceive;

    components NeighborDiscoveryC;
    Node.NeighborDiscovery-> NeighborDiscoveryC.NeighborDiscovery;

    components RoutingC;
    Node.Routing-> RoutingC.Routing;

    Node.RouteSender -> RoutingC.SimpleSend;
    Node.RouteReceived -> RoutingC.MainReceive;
    Node.RouteReplyReceive-> RoutingC.ReplyReceive;



    components TransportC;
    Node.Transport-> TransportC.Transport;

    Node.TransSender -> TransportC.SimpleSend;
    Node.TransReceived-> TransportC.MainReceive;
    Node.TransReplyReceive-> TransportC.ReplyReceive;

    components new HashmapC(socket_t*, 20);
    Node.Hash -> HashmapC;

    components new PoolC(socket_addr_t, 20);
    Node.addr_pool -> PoolC;

    components new QueueC(conn , 20);
    Node.QueueS -> QueueC;
    Node.QueueC -> QueueC;







}




















/*
configuration NodeC{
}
implementation {
    components MainC;
    components Node;
    components new AMReceiverC(AM_PACK) as GeneralReceive;

    Node -> MainC.Boot;

    Node.Receive -> GeneralReceive;

    components ActiveMessageC;
    Node.AMControl -> ActiveMessageC;

    components new SimpleSendC(AM_PACK);
    Node.Sender -> SimpleSendC;

    components CommandHandlerC;
    //components new AMReceiverC(AM_COMMANDMSG) as CommandReceive;

    Node.CommandHandler -> CommandHandlerC;
    //CommandHandlerC.Receive -> CommandReceive;

   components FloodingC;
  Node.FloodSend -> FloodingC.FloodSend;

>>>>>>> Stashed changes
}
*/
