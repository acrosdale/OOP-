#include "../../includes/packet.h"

interface Routing{
	command void start();
	command void print();
	command Router * getRTable();
	command void trans (pack msg, uint16_t dest, uint8_t protocol);

}
