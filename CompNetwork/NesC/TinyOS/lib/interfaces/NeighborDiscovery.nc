#include "../../includes/NeighborDiscovery_struct.h"

interface NeighborDiscovery{
	command void start();
	command void print();
	command neighbor* getNeighborList ();
	command neighbor* getNeighborListCHANGE ();
}
