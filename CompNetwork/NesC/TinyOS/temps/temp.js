while (e < 2) {
    for (z = 0; z < TO_WRITE;) { //who can write write

        if (temp.start < temp.finish) {

            if (temp.APP_buff[temp.start % BUFFER_SIZE] == 0) { //skip junk char
                //dbg(TRANSPORT_CHANNEL, "                        op done 2\n");

            } else {


                S_BUFF[z] = temp.APP_buff[temp.start % BUFFER_SIZE];
                dbg(TRANSPORT_CHANNEL, "app %c \n", temp.APP_buff[temp.start % BUFFER_SIZE]);

                if ((char) temp.APP_buff[temp.start % BUFFER_SIZE] == '\n') {

                    dbg(TRANSPORT_CHANNEL, "                         OP done %hhu \n", temp.fd - > Dest_port);
                    y = 0;
                    call Transport.write(temp.fd, S_BUFF, TO_WRITE);

                    while (y < call QueueS.size() && call QueueS.size() > 0) { //write to every on else in QueueS

                        all = call QueueS.element(y);
                        dbg(TRANSPORT_CHANNEL, "                       OPing %hhu \n", all.fd - > Dest_port);
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
        dbg(TRANSPORT_CHANNEL, "                       OPing %hhu \n", all.fd - > Dest_port);
        call Transport.write(all.fd, S_BUFF, TO_WRITE);

        y++;
    }
    e++;
}
