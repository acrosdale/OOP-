
		//cout<<"1  PRINT"<<endl;
		//cout<< schemaLeft<<endl;
		//cout<<endl;
		//cout<< schemaRight<<endl;\


		//this check to see how has the highest amount of comment
		//cout<<"TRACK "<<track2<<endl;
		//track2++;
		//cout<< "toBE JOINED"<<endl;
		//std::cout << "RIGH" <<schemaRight<<"\n"<< schemaRight.GetMaxTuples(schemaRight) << '\n';
		//std::cout << "LEFT" <<schemaLeft<<"\n"<< schemaLeft.GetMaxTuples(schemaLeft)<< '\n';
		if (schemaRight.GetMaxTuples(schemaRight) >= schemaLeft.GetMaxTuples(schemaLeft)) {
			leftSmaller = true;
			//cout<<"LEFT<<<<<<<<<<<<<<<<<<<<<<<<<<<<,"<<leftSmaller<<endl;
			//std::cout << "RIGH" << schemaRight.GetMaxTuples(schemaRight) << '\n';
			//std::cout << "LEFT" << schemaLeft.GetMaxTuples(schemaLeft)<< '\n';
		}

		//cout << "JOIN1" << endl;
		while (left->GetNext(temp)) {
			//cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<track++<< "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
			//temp.print(cout, schemaLeft);
			//	cout<<endl;
			//	cout << endl;
			DBFile _file;
			b = temp.GetBits();
			if (L_PageSizeInBytes + ((int*)b)[0] > (PAGE_NUM* PAGE_SIZE)) { // MeM cap reached store to disk

				L_files.push_back(_file);

				string Path = "SortedFile/lRun" + std::to_string(left_sublist_count) + ".Sorted";

				if (L_files[left_sublist_count].Create(const_cast<char*>(Path.c_str()), FileType::Sorted) == 0) {

					if (L_files[left_sublist_count].Open(Path.c_str()) == 1) {

						Record ToAdd;

						for (std::multimap<Record, int>::iterator it = Lrec.begin(); it != Lrec.end(); ++it) {
							ToAdd = (*it).first;

							L_files[left_sublist_count].AppendRecord(ToAdd);
						}

						Lrec.clear();
					}

					L_files[left_sublist_count].Close();
					left_sublist_count++;
					L_Mex = true;
					L_PageSizeInBytes = 0;
				}
			}
			L_PageSizeInBytes += ((int *)b)[0];
			temp.SetOrderMaker(_omL);
			Lrec.insert(pair<Record, int>(temp, 0));
		}


		//cout << "JOIN2" << endl;
		while (right->GetNext(Rec)) {
			//Rec.print(cout, schemaRight);
			//	cout << endl;

			DBFile file;
			DBFile file1;
			b = Rec.GetBits();

			if (R_PageSizeInBytes + ((int*)b)[0] > (PAGE_NUM * PAGE_SIZE)) { // MeM cap reached store to disk

				if (Lrec.size() > 0 && (L_Mex == false || L_Mex == true)) {
					L_Mex = true;
					L_files.push_back(file);

					cout<<"IM IN RIGHT GONA CREATE LEFT"<<Lrec.size()<<endl;


					string Path = "SortedFile/lRun" + std::to_string(left_sublist_count) + ".Sorted";

					if (L_files[left_sublist_count].Create(const_cast<char*>(Path.c_str()), FileType::Sorted) == 0) {

						if (L_files[left_sublist_count].Open(Path.c_str()) == 1) {

							Record ToAdd;

							for (std::multimap<Record, int>::iterator it = Lrec.begin(); it != Lrec.end();++it ) {
								ToAdd = (*it).first;

								L_files[left_sublist_count].AppendRecord(ToAdd);
							}
							Lrec.clear();
						}
						L_files[left_sublist_count].Close();
						//left_sublist_count++;
						L_Mex = true;
						//L_PageSizeInBytes = 0;
					}
				}

				DBFile NEW;
				R_files.push_back(NEW);

				string _Path = "SortedFile/rRun" + std::to_string(right_sublist_count) + ".Sorted";
				cout<< _Path<<endl;

				if (R_files[right_sublist_count].Create(const_cast<char*>(_Path.c_str()), FileType::Sorted) == 0) {

					if (R_files[right_sublist_count].Open(_Path.c_str()) == 1) {

						Record Add;

						for (std::multimap<Record, int>::iterator it = Rrec.begin(); it != Rrec.end(); ++it) {
							Add = (*it).first;

							R_files[right_sublist_count].AppendRecord(Add);
						}
						Rrec.clear();
					}

					R_files[right_sublist_count].Close();
					right_sublist_count++;
					R_Mex = true;
					R_PageSizeInBytes = 0;
				}
			}
			R_PageSizeInBytes += ((int *)b)[0];
			Rec.SetOrderMaker(_omR);
			Rrec.insert(pair<Record, int>(Rec, 0));
		}
		/*
		//cout << "JOIN3" << endl;
		if (Rrec.size() > 0 && R_Mex == true) {

			R_files.push_back(Blank);

			string _Path = "SortedFile/rRun" + std::to_string(right_sublist_count) + ".Sorted";

			if (R_files[right_sublist_count].Create(const_cast<char*>(_Path.c_str()), FileType::Sorted) == 0) {

				if (R_files[right_sublist_count].Open(_Path.c_str()) == 1) {

					Record Add;

				//	for (std::multimap<Record, int>::iterator _it = Rrec.begin(); _it != Rrec.end(); ) {
						//Add = (*_it).first;
						//_it =it;
						//R_files[right_sublist_count].AppendRecord(Add);
					//	++_it;
						//Rrec.erase(_it);

					//}

					//Rrec.clear();
				}

				R_files[right_sublist_count].Close();
				right_sublist_count++;
				//R_Mex = true;
				//R_PageSizeInBytes = 0;
			}
		}

		 if( L_Mex == true && Rrec.size() > 0  && R_Mex == false && right_sublist_count == 0){

			R_files.push_back(Blank);

			string _Path = "SortedFile/rRun" + std::to_string(right_sublist_count) + ".Sorted";

			if (R_files[right_sublist_count].Create(const_cast<char*>(_Path.c_str()), FileType::Sorted) == 0) {

				if (R_files[right_sublist_count].Open(_Path.c_str()) == 1) {
					Record Add;

				//	for (std::multimap<Record, int>::iterator it = Rrec.begin(); it != Rrec.end(); 	++it) {
						//Add = (*it).first;
						//R_files[right_sublist_count].AppendRecord(Add);

				//	}

					//Rrec.clear();
				}

				R_files[right_sublist_count].Close();
				right_sublist_count++;
				R_Mex = true;
				R_PageSizeInBytes = 0;

			}

		}
*/
		//cout << "JOIN4" << endl;


		join = true;
		cout<<"2  PRINT"<<endl;
		cout<< schemaLeft<<endl;
		cout<<endl;
		cout<< schemaRight<<endl;
		cout << "FINISHED JOIN" << endl;
		if(L_files.size() > 0 && R_files.size() > 0){
			int l = 0;
			cout << "MY TOTAL LEFT IS " << L_files.size() << endl;
			for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {

				string Path = "SortedFile/lRun" + std::to_string(l) + ".Sorted";
				l++;
				//(*it).Open(Path.c_str());

				//if ((*it).Open(Path.c_str()) == 1) {

				//	continue;
			//	}
				//else {
				//	cout << "OPENINIG SORTED FILE FAIL";
				//}

			}

			int r = 0;
			cout << "MY TOAL RIGHT IS " << R_files.size() << endl;
			for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {

				string _Path = "SortedFile/rRun" + std::to_string(r) + ".Sorted";
				r++;
				//if ((*it).Open(_Path.c_str()) == 1) {
///
				//	continue;
			//	}
				//else {
				//	cout << "OPENINIG SORTED FILE FAIL";
				//}
			}


		}

		if(Rrec.size()> 0){
				R_it = Rrec.begin();
		}
		if(Lrec.size()> 0){
				L_it = Lrec.begin();
		}

		//cout<<"IM GOIN IN"<<endl;
