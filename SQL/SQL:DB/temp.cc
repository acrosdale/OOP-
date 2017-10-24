
	else{

		if(left_sublist_count > 0 && right_sublist_count > 0 && option1 == false){

			cout<<"schemaLeft"<<endl;
			cout<< schemaLeft<<endl;
			/*
				cout<<"List count R L "<< right_sublist_count << " "<< left_sublist_count<<endl;
				cout<<"REC count L R "<< Lrec.size() << " "<< Rrec.size()<<endl;

				Create_col(left_sublist_count,right_sublist_count);

				if(left_sublist_count <= right_sublist_count){
					option1 =true; // left smaller table
					cout<<"REC count L R "<< l_chunks.size() << " "<< Rrec.size()<<endl;
				}
				else{
					option2 = true;
				}
*/
				// if out memory left rec and right rec should be 0

/*
			GET_NEXT_R_MIN:
			if(option2 == true && r_chunks[0].size() > 0 && Lrec.size() > 0 && JoinReady== false ){


				abs_right_min  = (*r_chunks[0].begin()).first; // get the smallest value
				std::vector<int> replacements;

				multimap<Record,int>::iterator find_all = r_chunks[0].find(abs_right_min); // find all samllest

				for(multimap<Record,int>::iterator it = find_all; it != r_chunks[0].end(); ++it){

					right_mins.push_back((*it).first);
					replacements.push_back((*it).second);

				}


				r_chunks[0].erase(abs_right_min);

				for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i],1);
				}
				replacements.clear();


				multimap<Record,int>::iterator find_left = Lrec.find(abs_right_min);


				if(find_left != Lrec.end()){

					while (true ){

						multimap<Record,int>::iterator k = Lrec.begin();
						Record _k =(*k).first;
						int result =  _omR->Run2ElectricBoogaloo(abs_right_min,_k, *_omL);

						if(result == 0){

							break;
						}else{ //less

							int indexes =(*Lrec.begin()).second;
							Lrec.erase( Lrec.begin() );
							GetNextRow(indexes, 2);

						}
					}

					multimap<Record,int>::iterator fin= Lrec.find(abs_right_min);

					for(multimap<Record,int>::iterator it = fin; it != Lrec.end(); ++it){

						left_mins.push_back((*it).first);
						replacements.push_back((*it).second);

					}
					Record del =left_mins[0];
					Lrec.erase(del);

					for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i],5);
					}

				}
				//V_L_it = left_mins.begin();
			    //V_R_it = right_mins.begin();
				JoinReady =true;
			} // the column are ready


			if(option1 == true && l_chunks[0].size() > 0 && Rrec.size() > 0 && JoinReady== false ){ // the column are ready

				//whichTableOP 0: when the left is the min table, when caled it fill left min with mins and l_chunck with potential min
				//whichTableOP 1: this vice versa
				//whichTableOP 2: when left_min > right_min
				//whichTableOP 3: when left_min < right_min

				abs_left_min  = (*l_chunks[0].begin()).first; // get the smallest value
				std::vector<int> replacements;

				multimap<Record,int>::iterator find_all = l_chunks[0].find(abs_left_min); // find all samllest

				for(multimap<Record,int>::iterator it = find_all; it != l_chunks[0].end(); ++it){

					left_mins.push_back((*it).first);
					replacements.push_back((*it).second);

				}

				l_chunks[0].erase(abs_left_min);

				for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i],0);
				}
				replacements.clear();


				multimap<Record,int>::iterator find_right = Rrec.find(abs_left_min);
				//multimap<Record,int>::iterator todel =Rrec.begin();

				if(find_right != Rrec.end()){

					while (true ){

						multimap<Record,int>::iterator k = Rrec.begin();
						Record _k =(*k).first;
						int result =  _omL->Run2ElectricBoogaloo(abs_left_min,_k, *_omR);

						if(result == 0){

							break;
						}else{ //less
							int indexes =(*Rrec.begin()).second;
							Rrec.erase( Rrec.begin() );

							GetNextRow(indexes, 3);

						}
					}

					multimap<Record,int>::iterator fin= Rrec.find(abs_left_min);

					for(multimap<Record,int>::iterator it = fin; it != Rrec.end(); ++it){

						right_mins.push_back((*it).first);
						replacements.push_back((*it).second);

					}
					Record del =right_mins[0];
					Rrec.erase(del);

					for(int i =0; i< replacements.size(); i++){ // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i],4);
					}

				}

			JoinReady =true;

			}

			if(JoinReady == true){

				for(int i = 0; i < right_mins.size();i++){

					for(int j = 0; j < left_mins.size();j++){

						//cout << "CHECKING JOIN" << endl;
						if (predicate.Run(left_mins[j], right_mins[i]) == true) {
							_record.AppendRecords(left_mins[j], right_mins[i], schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());
							r_join = i + 1;
							l_join =j+ 1;
							return true;
						}
					}
				}

				JoinReady =false;

				goto GET_NEXT_R_MIN;

			}*/
	}
