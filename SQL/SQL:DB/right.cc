/****************************DO SAME SHIT FOR RIGH SIDE********************************/

if(option2== true && leftbucket.size() > 0 && rightbucket.size() > 0 && JoinReady== false ){ // the column are ready
    //cout<<"RIGHT SIZE"<<endl;
    //whichTableOP 0: when the left is the min table, when caled it fill left min with mins and l_chunck with potential min
    //whichTableOP 1: this vice versa
    //whichTableOP 2: when left_min > right_min
    //whichTableOP 3: when left_min < right_min
    multimap<Record,string>::iterator _it;
    abs_right_min  = (*rightbucket.begin()).first; // get the smallest value

    std::vector<string> replacements;

    Record Z =(*leftbucket.begin()).first;

    if(abs_right_min.IsEqual( Z) == true ){
        // PROCEED as planned
        abs_left_min =(*leftbucket.begin()).first;
    }
    else if(abs_right_min.LessThan(Z) == true ){

        // get new left R < L;
        multimap<Record,string>::iterator _it;
        for(multimap<Record,string>::iterator it =rightbucket.begin(); it != rightbucket.end(); ){
            Record no = (*it).first;

            if(abs_right_min.IsEqual(no) == true){
                //left_mins.push_back((*it).first);
                replacements.push_back((*it).second);
                _it = it;
            }else{
                //thier not equal so it GreaterThan
                for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

                    GetNextRow(replacements[i],1,abs_right_min );
                }
                replacements.clear();
                break;
            }

            it++;
            rightbucket.erase(_it);
        }
        goto GET_NEXT_MINS;;
    }
    else{
        //R > L;
        Record current_L_min = (*leftbucket.begin()).first;

        for(multimap<Record,string>::iterator it =leftbucket.begin(); it != leftbucket.end(); ){
            Record no = (*it).first;

            if(current_L_min.IsEqual(no) == true){
                //left_mins.push_back((*it).first);
                replacements.push_back((*it).second);
                _it = it;
            }else{
                //thier not equal so it GreaterThan
                for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

                    GetNextRow(replacements[i],0,current_L_min);
                }
                replacements.clear();
                break;
            }

            it++;
            leftbucket.erase(_it);
        }
        goto GET_NEXT_MINS;;

    }


    // GET ALL MIN FROM RIGHT

    for(multimap<Record,string>::iterator it = rightbucket.begin(); it != rightbucket.end(); ){
        Record no = (*it).first;

        if(abs_right_min.IsEqual(no)){
            right_mins.push_back((*it).first);
            replacements.push_back((*it).second);
            _it =it;
        }else{
            //thier not equal so it GreaterThan
            for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

                GetNextRow(replacements[i],2,abs_right_min);
            }
            replacements.clear();
            break;
        }

        ++it;
        rightbucket.erase(_it);
    }

    //GET ALL MIN FROM LEFT

    for(multimap<Record,string>::iterator it = leftbucket.begin(); it != leftbucket.end(); ){
        Record no = (*it).first;

        if(abs_left_min.IsEqual(no)){
            left_mins.push_back((*it).first);
            replacements.push_back((*it).second);
            _it =it;
        }else{
            //thier not equal so it GreaterThan
            for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

                GetNextRow(replacements[i],3,abs_left_min);
            }
            replacements.clear();
            break;
        }

        ++it;
        leftbucket.erase(_it);
    }

    //start Join
    if(left_mins.size() > 0 && right_mins.size() > 0){ // i got some min lets join
        JoinReady = true;
        left_mins_it = left_mins.begin();
        right_mins_it = right_mins.begin();
    }

}
