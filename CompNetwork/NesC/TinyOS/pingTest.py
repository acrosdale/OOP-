from TestSim import TestSim

def main():
    # Get simulation ready to run.
    s = TestSim();

    # Before we do anything, lets simulate the network off.
    s.runTime(1);

    # Load the the layout of the network.
    s.loadTopo("long_line.topo");

    # Add a noise model to all of the motes.
    s.loadNoise("no_noise.txt");

    # Turn on all of the sensors.
    s.bootAll();

    # Add the main channels. These channels are declared in includes/channels.h
    s.addChannel(s.COMMAND_CHANNEL);
    s.addChannel(s.GENERAL_CHANNEL);
    s.addChannel(s.FLOODING_CHANNEL);
    s.addChannel(s.NEIGHBOR_CHANNEL);
    s.addChannel(s.ROUTING_CHANNEL);
    s.addChannel(s.TRANSPORT_CHANNEL);



    """s.runTime(10);

    print("\nALL MOTES ARE ON\n")

    s.routeDMP(1);
    s.runTime(700);

    s.ping(1, 6, "Hi!"); ##flood the network
    s.runTime(100);

    s.moteOff(4)
    s.runTime(1000);

    print("\n4 MOTES ARE off\n")

    s.routeDMP(1);
    s.runTime(100);

    s.ping(1, 6, "Hi!"); ##flood the network
    s.runTime(1000);

    print("\n1,4 MOTES ARE on\n")
    s.moteOn(4)
    s.runTime(1000);

    s.routeDMP(1);
    s.runTime(100);

    s.ping(1, 6, "Hi!"); ##flood the network
    s.runTime(1000);
    """


    s.runTime(300);
    s.testServer(3);
    s.runTime(60);

    s.chat(2,"hello acrosdale\r\n");
    s.runTime(500);

    s.chat(4,"hello alex\r\n");
    s.runTime(800);

    s.chat(1,"hello john\r\n");
    s.runTime(800);

    s.chat(4,"listusers\r\n");
    s.runTime(800);

    s.chat(2,"message hey guys!\r\n");
    s.runTime(500);





    """s.testClient(2);
    s.runTime(1);
    s.runTime(2000);"""

    """s.testClient(3);
    s.runTime(1);
    s.runTime(2000);
"""


    #s.neighborDMP(3);

    #s.ping(9, 1, "Hi!");
    #s.runTime(30);
    #s.neighborDMP(3);
    #s.runTime(15);
    #s.moteOff(6);
    #s.runTime(1);
if __name__ == '__main__':
    main()
