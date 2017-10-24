
			// if out memory left rec and right rec should be 0
	GET_NEXT_MINS:
	FIND_NEW_L_MIN:
			if(option1 == true && leftbucket.size() > 0 && rightbucket.size() > 0 && JoinReady== false ){ // the column are ready

				//whichTableOP 0: when the left is the min table, when caled it fill left min with mins and l_chunck with potential min
				//whichTableOP 1: this vice versa
				//whichTableOP 2: when left_min > right_min
				//whichTableOP 3: when left_min < right_min

				abs_left_min  = (*leftbucket.begin()).first; // get the smallest value

				std::vector<string> replacements;

				multimap<Record,string>::iterator find_right = rightbucket.find(abs_left_min);

				if(find_right != rightbucket.end()){

					while (true ){

						multimap<Record,string>::iterator k = rightbucket.begin();
						Record _k =(*k).first;
						int result =  _omL->Run2ElectricBoogaloo(abs_left_min,_k, *_omR);

						if(result == 0){

							break;
						}else{ //less
							string indexes =(*rightbucket.begin()).second;
							rightbucket.erase( rightbucket.begin() );

							GetNextRow(indexes, 3,abs_left_min);

						}
					}

					right_mins.push_back( (*rightbucket.begin()).first );
					multimap<Record,string>::iterator to = ++rightbucket.begin();

					for(multimap<Record,string>::iterator it = to; it != rightbucket.end(); ++it){
						Record no = (*it).first;

						if(abs_left_min.IsEqual(no)){
							right_mins.push_back((*it).first);
							replacements.push_back((*it).second);
						}else{
							break;
						}

					}

					Record del =right_mins[0];
					Rrec.erase(del);

					for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i],4,abs_left_min);
					}

				}
				else{
					//bad left min get a new one
					leftbucket.erase(abs_left_min);
					goto FIND_NEW_L_MIN;
				}


				left_mins.push_back(abs_left_min);

				for(multimap<Record,string>::iterator it = ++leftbucket.begin(); it != leftbucket.end(); ++it){
					Record no = (*it).first;

					if(abs_left_min.IsEqual(no)){
						left_mins.push_back((*it).first);
					}

				}
				cout<<"BEFORE LEFT BUCKET"<<leftbucket.size()<< " "<<replacements.size()<<endl;
				leftbucket.erase(abs_left_min);
				cout<<"AFTER LEFT BUCKET"<<leftbucket.size()<< " "<<replacements.size()<<endl;

				for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i],0,abs_left_min);
				}
				replacements.clear();

				if(left_mins.size() > 0 && right_mins.size() > 0){
					JoinReady =true;
					left_mins_it = left_mins.begin();
					right_mins_it = right_mins.begin();
					cout<<"LSIZE  RSIZE"<< left_mins.size()<<" "<<right_mins.size()<<endl;
				}
			}


			if(JoinReady == true){

				while( right_mins_it != right_mins.end() ){

					while( left_mins_it != left_mins.end() ){

						//cout << "CHECKING JOIN" << endl;
						Record right= (*right_mins_it);
						Record left =(*left_mins_it);

						left_mins_it++;
						if(left_mins_it == left_mins.end() ){
							left_mins_it =left_mins.begin();
							right_mins_it++;
						}
						if (predicate.Run(left, right) == true) {

							_record.AppendRecords(left, right, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());

							return true;
						}
					}
				}

				JoinReady =false;
				right_mins.clear();
				left_mins.clear();
				goto GET_NEXT_MINS;

			}
