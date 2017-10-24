FIND_NEW_R_MIN:
	 		if(option2 == true && rightbucket.size() > 0 && leftbucket.size() > 0 && JoinReady== false ){

				abs_right_min  = (*rightbucket.begin()).first; // get the smallest value
				std::vector<string> replacements;

				multimap<Record,string>::iterator find_left = leftbucket.find(abs_right_min);

				if(find_left != leftbucket.end()){

					while (true ){

						multimap<Record,string>::iterator k = leftbucket.begin();
						Record _k =(*k).first;
						int result =  _omR->Run2ElectricBoogaloo(abs_right_min,_k, *_omL);

						if(result == 0){

							break;
						}else{ //less

							string indexes =(*leftbucket.begin()).second;
							leftbucket.erase( leftbucket.begin() );
							GetNextRow(indexes, 2,abs_right_min);

						}
					}

					multimap<Record,string>::iterator fin= leftbucket.find(abs_right_min);

					for(multimap<Record,string>::iterator it = fin; it != leftbucket.end(); ++it){

						left_mins.push_back((*it).first);
						replacements.push_back((*it).second);

					}
					Record del =left_mins[0];
					leftbucket.erase(del);

					for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i],5,abs_right_min);
					}


				}else{
					//bad left min get a new one
					rightbucket.erase(abs_right_min);
					goto FIND_NEW_R_MIN;
				}

				multimap<Record,string>::iterator find_all = rightbucket.find(abs_right_min); // find all samllest

				for(multimap<Record,string>::iterator it = find_all; it != rightbucket.end(); ++it){

					right_mins.push_back((*it).first);
					replacements.push_back((*it).second);

				}

				rightbucket.erase(abs_right_min);

				for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i],1,abs_right_min);
				}
				replacements.clear();


				if(left_mins.size() > 0 && right_mins.size() > 0){
					JoinReady =true;
					left_mins_it = left_mins.begin();
					right_mins_it = right_mins.begin();
				}

			} // the column are ready
