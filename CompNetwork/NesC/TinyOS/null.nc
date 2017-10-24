
            while(j < read){

                a=(temp.read_buff[j+1]<< 8 ) | (temp.read_buff[j] & 0xff);
                //dbg(TRANSPORT_CHANNEL, "Reading# %c \n", a);

                if(a == 'h' && temp.fd->active == FALSE ){

                    b=(temp.read_buff[j+3]<< 8 ) | (temp.read_buff[j+2] & 0xff);

                    if(b == 'e'){

                        c=(temp.read_buff[j+5]<< 8 ) | (temp.read_buff[j+4] & 0xff);

                        if(c== 'l'){

                            d=(temp.read_buff[j+7]<< 8 ) | (temp.read_buff[j+6] & 0xff);
                            if(d =='l'){
                                over_read=0;

                                temp.start = over_read;  // start a read counter
                                //dbg(TRANSPORT_CHANNEL, "Reading@# %hhu \n", over_read);
                                option = 1; //take in hello comand message
                                temp.fd->active == TRUE;
                                while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer
                                j1=0;
                            }
                        }

                    }

                }
                else if(a == 'm'){

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
                                //dbg(TRANSPORT_CHANNEL, "AAAA \n");
                                option = 5; //take in message c
                                while(j1 < 128){ temp.APP_buff[(temp.start +j1)%BUFFER_SIZE] =0; j1++; } //clean the APP buffer
                                j1=0;

                            }
                        }
                    }

                }

                if(option ==1 ){  // get name and port  for a respose

                    while( j1 < 12){

                        a=(temp.read_buff[j1+1]<< 8 ) | (temp.read_buff[j1] & 0xff);
                        //dbg(TRANSPORT_CHANNEL, "AAAA %c\n",a);
                        if((char)a == '\r'){
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




            if(option == 2){ //if any has anything to write do it

                //dbg(TRANSPORT_CHANNEL, "         enter opt %hhu \n",option);
                for( z = 0; z < TO_WRITE;){

                    if(temp.start < temp.finish){

                        if(temp.APP_buff[temp.start%BUFFER_SIZE] == 0){//skip over junk char
                            //dbg(TRANSPORT_CHANNEL, "                        op done 2\n");
                            temp.start++;
                            //continue;
                        }

                        dbg(TRANSPORT_CHANNEL, "app %c \n",temp.APP_buff[temp.start%BUFFER_SIZE]);


                        S_BUFF[z]   = temp.APP_buff[temp.start%BUFFER_SIZE] & 0xff;
                        S_BUFF[z+1] = temp.APP_buff[temp.start%BUFFER_SIZE] >> 8;

                        temp.start++;

                        if( (char)temp.APP_buff[temp.start%BUFFER_SIZE] == '\n'){

                            S_BUFF[z]   = temp.APP_buff[temp.start%BUFFER_SIZE] & 0xff;
                            S_BUFF[z+1] = temp.APP_buff[temp.start%BUFFER_SIZE] >> 8;


                            //dbg(TRANSPORT_CHANNEL, "                       OP done \n");
                            option =0; break;

                        }

                    }

                    z+=2;
                }
                dbg(TRANSPORT_CHANNEL, "                       OP done  %hhu\n",temp.fd->Dest_addr );
                call Transport.write(temp.fd, S_BUFF,TO_WRITE);

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

            if (option == 4) { //send msg to ever one
                conn all;
                uint16_t y = 0;
                over_read = 0;


                for (z = 0; z < TO_WRITE;) { //who can write write

                    if (temp.start < temp.finish) {

                        if (temp.APP_buff[temp.start % BUFFER_SIZE] == 0) { //skip junk char
                            //dbg(TRANSPORT_CHANNEL, "                        op done 2\n");

                        } else {

                            dbg(TRANSPORT_CHANNEL, "app %c \n", temp.APP_buff[temp.start % BUFFER_SIZE]);

                            S_BUFF[z] = temp.APP_buff[temp.start % BUFFER_SIZE] & 0xff;
                            S_BUFF[z + 1] = temp.APP_buff[temp.start % BUFFER_SIZE] >> 8;


                            if ((char) temp.APP_buff[temp.start % BUFFER_SIZE] == '\n') {

                                //dbg(TRANSPORT_CHANNEL, "                         OP done %hhu \n",temp.fd->Dest_port);
                                call Transport.write(temp.fd, S_BUFF, TO_WRITE);
                                option = 0;
                                break;

                            }

                        }

                        temp.start++;
                    }

                    z += 2;
                }

                while (y < call QueueS.size()) { //write to every on else in QueueS

                    all = call QueueS.element(y);
                    //dbg(TRANSPORT_CHANNEL, "                       OPing %hhu \n",all.fd->Dest_port);
                    call Transport.write(all.fd, S_BUFF, TO_WRITE);

                    y++;
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
                            if (track == 3) {
                                break;
                            }
                            if (temp.name[l] != 0) {

                                temp.APP_buff[(j1 + l) % BUFFER_SIZE] = temp.name[l - 1];
                                //dbg(TRANSPORT_CHANNEL, "                       OPing000 %c \n", temp.name[l]);

                            }
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

                                if (user.name[c] == 0) {
                                    track++;
                                    c++;
                                }
                                if (track == 3) {
                                    break;
                                }
                                if (user.name[c] != 0) {
                                    //dbg(TRANSPORT_CHANNEL, "                       OPing2 %c \n", temp.name[l]);
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

                j1 = 0;
                while (j1 < temp.finish) {

                    dbg(TRANSPORT_CHANNEL, "     APP BUFF %c \n", temp.APP_buff[(j1) % BUFFER_SIZE]);

                    j1++;
                }
                option = 6;
            }

            if (option == 6) {
                conn all;
                uint16_t y = 0;
                over_read = 0;


                for (z = 0; z < TO_WRITE;) {

                    if (temp.start < temp.finish) {

                        if (temp.APP_buff[temp.start % BUFFER_SIZE] == 0) {
                            //dbg(TRANSPORT_CHANNEL, "                        op done %hhu\n", temp.start);
                            //temp.start+=1;
                            //dbg(TRANSPORT_CHANNEL, "                        op done %hhu\n", temp.start);
                            //continue;
                        } else {
                            dbg(TRANSPORT_CHANNEL, "app %c \n", temp.APP_buff[temp.start % BUFFER_SIZE]);

                            S_BUFF[z] = temp.APP_buff[temp.start % BUFFER_SIZE] & 0xff;
                            S_BUFF[z + 1] = temp.APP_buff[temp.start % BUFFER_SIZE] >> 8;


                            if ((char) temp.APP_buff[temp.start % BUFFER_SIZE] == '\n') {

                                //dbg(TRANSPORT_CHANNEL, "                         OP done %hhu \n",temp.fd->Dest_port);
                                //call Transport.write(temp.fd, S_BUFF,TO_WRITE);
                                option = 0;
                                break;

                            }
                        }

                        temp.start++;
                    }

                    z += 2;
                }

                //call Transport.write(temp.fd, S_BUFF,TO_WRITE); //write to the one in use

            }
