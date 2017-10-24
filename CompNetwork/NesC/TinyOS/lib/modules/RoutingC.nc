#include <Timer.h>

#define AM_ROUTING 21

#define AM_ROUTING2 12

configuration RoutingC{


	provides interface Routing;

	provides interface SimpleSend;
	provides interface Receive as MainReceive;
	provides interface Receive as ReplyReceive;



}

implementation{
	components RoutingP;
	components TransportC;
	components NeighborDiscoveryC;
	components new TimerMilliC() as beaconTimer;
	components new TimerMilliC() as oneShot;

	components new SimpleSendC(AM_ROUTING) as SENDC;
    components new AMReceiverC(AM_ROUTING) as AMRC;

	components new SimpleSendC(AM_ROUTING2);
    components new AMReceiverC(AM_ROUTING2);

	// External Wiring
	Routing = RoutingP.Routing;

	// internal Wiring
	RoutingP.packetSending -> SimpleSendC;
	RoutingP.packetReceived -> AMReceiverC;


	RoutingP.InternalSending -> SENDC;
	RoutingP.InternalReceived -> AMRC;


	MainReceive  = RoutingP.MainReceive;
    ReplyReceive = RoutingP.ReplyReceive;
    SimpleSend   = RoutingP.RouteSender;



	RoutingP.beaconTimer -> beaconTimer;
	RoutingP.oneShot -> oneShot;
	RoutingP.NeighborDiscovery -> NeighborDiscoveryC;



	RoutingP.Transporter->TransportC.Transport;
}
