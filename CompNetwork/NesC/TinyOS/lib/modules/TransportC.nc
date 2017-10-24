#include <Timer.h>
#include "../../includes/packet.h"
#include "../../includes/socket.h"

#define AM_transport 23

#define AM_transport2 14

configuration TransportC{


	provides interface Transport;

	provides interface SimpleSend;
	provides interface Receive as MainReceive;
	provides interface Receive as ReplyReceive;

}

implementation{
	components TransportP;
	components RoutingC;
	components new TimerMilliC() as WaitTimer;
	components new TimerMilliC() as oneShot;
	components new TimerMilliC() as EFF_timer;

	//components new SimpleSendC(AM_transport) as TransC;
    //components new AMReceiverC(AM_transport) as AMRC;

	//components new SimpleSendC(AM_transport2);
    //components new AMReceiverC(AM_transport2);

	// External Wiring
	Transport = TransportP.Transport;

	// internal Wiring
	//TransportP.packetSending -> SimpleSendC;
	//TransportP.packetReceived -> AMReceiverC;


	//TransportP.InternalSending -> TransC;
	//TransportP.InternalReceived -> AMRC;


	MainReceive  = TransportP.MainReceive;
    ReplyReceive = TransportP.ReplyReceive;
    SimpleSend   = TransportP.transSender;


	//Lists
    components new PoolC(socket_t, 20);
    components new QueueC(transmission , 20);
	components new HashmapC(socket_t*, 20);

	TransportP.Pool -> PoolC;
	//TransportP.Pool_addr -> PoolC;
    TransportP.Queue -> QueueC;
	TransportP.Hash -> HashmapC;



	TransportP.WaitTimer -> WaitTimer;
	TransportP.oneShot -> oneShot;
	TransportP.EFF_timer-> EFF_timer;



	TransportP.Routing -> RoutingC;
}
