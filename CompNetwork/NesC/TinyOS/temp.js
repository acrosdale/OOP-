void
mergeRoute(Route * new) {
    int i;
    for (i = 0; i < numRoutes; ++i) {

        if (newRoute ->Dest == Table[i].Destination) {

            if (newRoute->Cost + 1 < routingTable[i].Cost) {
                /* found a better route: */
                break;
            } else if (newRoute ->NextHop == routingTable[i].NextHop) {
                /* metric for current next-hop may have
                changed: */
                break;

            } else {
                /* route is uninteresting---just ignore
                  it */
                return;
            }
        }

    }
    if (i == numRoutes) {
        /* this is a completely new route; is there room
           for it? */
        if (numRoutes < MAXROUTES) {
            ++numRoutes;
        } else {
            /* can't fit this route in table so give up */
            return;
        }
    }
    miniRTable[i] = * new;
    /* reset TTL */
    //miniRTableTable[i].TTL = MAX_TTL;
    /* account for hop to get to next node */
    ++miniRTable[i].Cost;
}
