#include <iostream>
#include "RelOp.h"

#include <stdio.h>
#include <string.h>

#include "EfficientMap.cc"
#include <map>
#include <list>
#include <stdlib.h>
#include "Record.h"
#include <sstream>
#include <regex>


#define PAGE_NUM 100

bool checkNaCu;

using namespace std;


static int Pages;


int tab = 0;
ScanIndex::ScanIndex(Schema& _schema, Schema& _toSendSch, DBFile& _index, DBFile& _header,
	CNF& _predicate, Record& _constants, string& _table, string& _attribute) {
	// schema of records in operator
	 schema = _schema;
	toSendSch = _toSendSch;

	// selection predicate in conjunctive normal form
	predicate = _predicate;
	// constant values for attributes in predicate
	constants = _constants;

	 table = _table;
	attribute = _attribute;
	//key;

	index = _index;
	header = _header;

	constants = _constants;
}

bool ScanIndex::GetNext(Record& _record) {
	//cout << "WE BE SCANNIN" << endl;

	if (header.GetNext(_record) == 1) {
		//cout << "TRUE" << endl;

		return true;
	}

	return false;

}


ScanIndex::~ScanIndex() {
}

ostream& ScanIndex::print(ostream& _os) {

	return _os << "\t" << "SCANINDEX ON ATT" << attribute;

}



ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}


Scan::Scan(Schema& _schema, DBFile& _file) {

	schema = _schema;
	file = _file;

}
bool Scan::GetNext(Record& _record) {
	//cout << "WE BE SCANNIN" << endl;

	if (file.GetNext(_record) == 1) {
		//cout << "TRUE" << endl;

		return true;
	}

	return false;

}

Scan::~Scan() {
}

ostream& Scan::print(ostream& _os) {

	return _os << "\t" << "SCAN " << schema.GetNumAtts() << " STARTING WITH " << schema.GetAtts()[0].name;

}

int mcnt = 0;

bool Select::GetNext(Record& _record) {
	//cout << "WE SELECTING" << endl;
	Record rec;
	//cout << producer->GetNext(rec) << endl;

	//cout << schema << endl;
	while (producer->GetNext(rec)) {
		//	cout << "LOOP" << endl;
		//cout << predicate.Run(_record, constants) << endl;
		//++mcnt;
		//if(mcnt == 5909864)
		//printf("here\n");
		if (predicate.Run(rec, constants)) {
			_record = rec;
			//cout << "predicate true" << endl;
			//cout << schema << endl;
			//constants.print(cout, schema);
			//_record = rec;
			return true;
		}
	}

	return false;
}


Select::Select(Schema& _schema, CNF& _predicate, Record& _constants,
	RelationalOp* _producer) {


	//cout << _schema << endl;
	//cout << endl;
	/*
	cout << _predicate.numAnds << endl;
	cout << _predicate << endl;
	cout << _predicate.andList[0] << endl;
	cout << _predicate.andList[0].whichAtt1 << endl;  //where in first att

	cout << _predicate.andList[0].whichAtt2; //where in 2nd att
	cout << _predicate.andList[0].operand2 << endl;
	cout << "Constants size "<< _constants.GetSize() << endl;
	cout << _predicate.andList[0].attType << endl;
	cout << _predicate.andList[0];
	char*bits;
	string bitss = _constants.GetBits();
	//_constants.
	char * bits2 = _constants.GetBits();
	_constants.CopyBits(bits,1);

	cout << "BITS" << endl;
	cout << bits << endl;
	cout << bitss << endl;
	cout << bits[0] << endl;
	cout << bits[1] << endl;
	cout << bits2[0] << endl;
	*/

	/*
	vector<int> remove;
	for (int i = 0; i < _predicate.numAnds; i++) {
	remove.push_back(_predicate.andList[i].whichAtt1);
	}

	_schema.Project(remove);
	*/

	//cout << endl;
	//cout << _schema << endl;
	//char * bits=_constants.GetBits();
	//	cout << << endl;
	//cout << *_producer << endl;


	schema = _schema;
	predicate = _predicate;
	constants = _constants;
	producer = _producer;
	//cout << schema << endl;

	//constants.print(cout, schema);
	//cout << _predicate << endl;

}

Select::~Select() {

}

ostream& Select::print(ostream& _os) {
	//string bitss (constants.);
	cout << "\tSELECT ";

	Schema temp = schema;
	/*
	vector<int> remove;
	for (int i = 0; i < predicate.numAnds; i++) {
	remove.push_back(predicate.andList[i].whichAtt1);
	}

	temp.Project(remove);
	constants.print(_os,temp);
	string s;
	for (int i = 0; i < predicate.numAnds; i++) {
	if (predicate.andList[i].op == LessThan)
	s += "< ";
	else if (predicate.andList[i].op == GreaterThan) {
	s += "> ";
	}
	else
	s += "= ";
	}
	*/
	return _os << " " << schema << *producer;

}


Project::Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
	int _numAttsOutput, int* _keepMe, RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	numAttsInput = _numAttsInput;
	numAttsOutput = _numAttsOutput;
	keepMe = _keepMe;
	producer = _producer;


	//cout << schemaIn << endl;
	//cout << schemaOut << endl;
	//cout << numAttsOutput << endl;
	//cout << numAttsInput << endl;
	//cout << endl;
}

bool Project::GetNext(Record& _myRec) {

	//first, call nextRecord from producer. true on success
	//	cout << "test" << endl;
	int * keep = new int[schemaOut.GetNumAtts()];
	vector<Attribute> findin = schemaIn.GetAtts();
	vector<Attribute> findout = schemaOut.GetAtts();
	int count = 0;

	//	cout << endl;
	//	cout << endl;
	//cout << "PROJECT" << endl;
	for (int i = 0; i < findin.size(); i++) {
		for (int j = 0; j < findout.size(); j++) {
			if (findin[i].name == findout[j].name) {
				//	cout << " FOUNDED " << endl;
				//	cout << findin[i].name << " " << findout[j].name << endl;
				//		cout << i << endl;
				keep[count] = i;
				count++;
			}
		}

	}
	if (producer->GetNext(_myRec)) {
		// success, continue
		//	cout << schemaIn << endl;
		//	cout << schemaOut << endl;
		//	cout << numAttsOutput << endl;
		//	cout << numAttsInput << endl;
		//	cout << endl;
		// Project (int* attsToKeep, int numAttsToKeep, int numAttsNow)
		/*
		cout << "BEFORE PROJECT" << endl;
		_myRec.print(cout, schemaOut);
		cout << endl;

		_myRec.Project(keep, numAttsOutput, numAttsInput);
		cout << "AFTER PROJECT" << endl;
		_myRec.print(cout, schemaOut);
		cout << endl;
		*/
		_myRec.Project(keep, numAttsOutput, numAttsInput);
		//	_myRec.print(cout, schemaOut);
		//	cout << "crash" << endl;
		return true;

	}

	//error
	cout << "ERROR" << endl;
	return false;

}



Project::~Project() {

}

ostream& Project::print(ostream& _os) {
	vector<Attribute> a;
	a = schemaOut.GetAtts();
	string s;
	for (int i = 0; i < a.size(); i++) {
		s += a[i].name;
		s += " ";
	}
	//cout << schemaIn << endl;
	//cout << schemaOut << endl;
	return _os << "PROJECT " << s << endl << "\t" << *producer;
}


Join::Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right, AndList& _parseTree) {

	schemaLeft = _schemaLeft;
	schemaRight = _schemaRight;
	schemaOut = _schemaOut;
	predicate = _predicate;
	left = _left;
	right = _right;


	Lrec = multimap<Record, int>();
	Rrec = multimap<Record, int>();



	//first phase
	_omL = new OrderMaker();
	_omR = new OrderMaker();

	predicate.GetSortOrders(*_omL, *_omR);

	join = false;
	L_Mex = false;
	R_Mex = false;
	leftSmaller = false;

	left_sublist_count = 0;
	right_sublist_count = 0;
	chunks_filled = false;
	L_PageSizeInBytes = 0;  //this is page limit when broken the rec will be sorted and stored in disk
	R_PageSizeInBytes = 0;

	option1 = false; //both in memory
	option2 = false;// left in right out
	JoinReady =false;

//	last_file_accessed = "Nothing";
	TOBE = false;
	abs_min_index=-1;
	once = false;
	once2 = false;
	notagain = false;

	//cout<< "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
}

bool Join::GetNext(Record& _record) {
	Record temp, Rec;

	int total = 1;

	int track  =1;
		int track2  =1;

	int count = 0;
	char * b;


	//cout << "JOIN" << endl;
	if (join == false) {

		if (schemaRight.GetMaxTuples(schemaRight) >= schemaLeft.GetMaxTuples(schemaLeft)) {
			leftSmaller = true;

		}


		while (left->GetNext(temp)){
				//temp.print(cout, schemaLeft);
				//cout<<endl;
			b = temp.GetBits();
			if (L_PageSizeInBytes + ((int*)b)[0] > (PAGE_NUM* PAGE_SIZE)) { // MeM cap reached store to disk
				DBFile _file;

				stringstream tempo;
				tempo<<&_file;

				string Path = "SortedFile/lRun_" + tempo.str() + "_"+ std::to_string(left_sublist_count) + ".Sorted";

				_file.SetName(Path);
				//L_files.push_back(_file);
				tempo.clear();

				if (_file.Create(const_cast<char*>(Path.c_str()), FileType::Sorted) == 0) {

					if (_file.Open(Path.c_str()) == 1) {

						Record ToAdd;

						for (std::multimap<Record, int>::iterator it = Lrec.begin(); it != Lrec.end(); ++it) {
							ToAdd = (*it).first;

							_file.AppendRecord(ToAdd);
						}

						cout<<Path <<" "<<Lrec.size()<<endl;
						Lrec.clear();
					}

					_file.Close();
					left_sublist_count++;
					L_Mex = true;
					L_files.push_back(_file);
					L_PageSizeInBytes = 0;
				}
			}
			L_PageSizeInBytes += ((int *)b)[0];
			temp.SetOrderMaker(_omL);
			Lrec.insert(pair<Record, int>(temp, 0));
		}

		while (right->GetNext(Rec)) {
		//	Rec.print(cout, schemaRight);
			//cout << endl;

			b = Rec.GetBits();

			if (R_PageSizeInBytes + ((int*)b)[0] > (PAGE_NUM * PAGE_SIZE)) { // MeM cap reached store to disk

				DBFile NEW;

				stringstream t;
				t<<&NEW;

				string _Path = "SortedFile/rRun_" + t.str()+ "_"+ std::to_string(right_sublist_count) + ".Sorted";
				cout<< _Path<<endl;

				NEW.SetName(_Path);
				//R_files.push_back(NEW);
				t.clear();


				if (NEW.Create(const_cast<char*>(_Path.c_str()), FileType::Sorted) == 0) {

					if (NEW.Open(_Path.c_str()) == 1) {

						Record Add;

						for (std::multimap<Record, int>::iterator it = Rrec.begin(); it != Rrec.end(); ++it) {
							Add = (*it).first;

							NEW.AppendRecord(Add);
						}
						Rrec.clear();
					}

					NEW.Close();
					R_files.push_back(NEW);
					right_sublist_count++;
					R_Mex = true;
					R_PageSizeInBytes = 0;
				}
			}
			R_PageSizeInBytes += ((int *)b)[0];
			Rec.SetOrderMaker(_omR);
			Rrec.insert(pair<Record, int>(Rec, 0));
		}


		if (Lrec.size() > 0 && L_Mex  == true) {

			DBFile file;

		//	cout<<"IM IN left GONA load last LEFT"<<Lrec.size()<<endl;
			stringstream temo;
			temo<<&file;

			string Path = "SortedFile/lRun_"+temo.str() + "_"+std::to_string(left_sublist_count) + ".Sorted";
			file.SetName(Path);
			//L_files.push_back(file);
			temo.clear();

			if (file.Create(const_cast<char*>(Path.c_str()), FileType::Sorted) == 0) {

				if (file.Open(Path.c_str()) == 1) {

					Record ToAdd;

					for (std::multimap<Record, int>::iterator it = Lrec.begin(); it != Lrec.end();++it ) {
						ToAdd = (*it).first;

						file.AppendRecord(ToAdd);
					}
					cout<<Path <<" "<<Lrec.size();
					Lrec.clear();
				}
				file.Close();
				//left_sublist_count++;
				L_files.push_back(file);
				L_Mex = true;
				L_PageSizeInBytes = 0;
			}
		}

		if (Rrec.size() > 0 && R_Mex == true) {
			DBFile NEWO;

		//	cout<<"IM IN Right GONA load last Right"<<Rrec.size()<<endl;

			stringstream t;
			t<<&NEWO;

			string _Path = "SortedFile/rRun_" + t.str()+ "_"+ std::to_string(right_sublist_count) + ".Sorted";

			NEWO.SetName(_Path);

			t.clear();


			if (NEWO.Create(const_cast<char*>(_Path.c_str()), FileType::Sorted) == 0) {

				if (NEWO.Open(_Path.c_str()) == 1) {

					Record Add;

					for (std::multimap<Record, int>::iterator _it = Rrec.begin(); _it != Rrec.end();++_it ) {
						Add = (*_it).first;

						NEWO.AppendRecord(Add);

					}

					Rrec.clear();
				}

				NEWO.Close();
				R_files.push_back(NEWO);
				right_sublist_count++;
				//R_Mex = true;
				R_PageSizeInBytes = 0;
			}
		}

		join = true;
		cout << "\nFINISHED JOIN" << endl;
		if(L_files.size() > 0  && L_Mex == true){

			for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {


				string Path =(*it).GetName();

				(*it).Open(Path.c_str());

				if ((*it).Open(Path.c_str()) == 1) {

					continue;
				}
				else {
					cout << "OPENINIG SORTED FILE FAIL";
				}

			}

		}
		if(R_files.size() > 0  && R_Mex == true){


			for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {


				string Path =(*it).GetName();

				if ((*it).Open(Path.c_str()) == 1) {

					continue;
				}
				else {
					cout << "OPENINIG SORTED FILE FAIL";
				}

			}

		}

		if(Rrec.size()> 0){
				R_it = Rrec.begin();
		}
		if(Lrec.size()> 0){
				L_it = Lrec.begin();
		}


	}

	/*

	*/



	/**
	**
	**    Merge phase
	**
	**/

	//cout << "MEX L R"<< Lrec.size()<< " "<<Rrec.size()<<endl;

	if(L_Mex == false && R_Mex == false){ // both in memory
		if(notagain == false){
			//cout<< "BOTH IN MEM>>>>>>>>>>>>>>>>>>>>>"<<endl;
			notagain =true;
		}



		if(RUNNER(0,_record) == true){

			return true;
		}

	}
	else if(L_Mex == true && R_Mex == false){ //left out right in
		if(notagain == false){
			//cout<< "VICE>>>>>>>>>>>>>>>>>>>>>>"<<endl;
			notagain =true;
		}

		if(once == false){
			V_L_it = L_files.begin();
			once = true;
		}
		if(RUNNER(1,_record) == true){

			return true;

		}
	}
	else if(L_Mex == false && R_Mex == true){ //left in right out

		if(notagain == false){
			//cout<< "FFFFF>>>>>>>>>>>>>>>>>>>>>>"<<endl;
			notagain =true;
		}

		if(once2 == false){
			V_R_it = R_files.begin();
			once2 = true;
		}
		if(RUNNER(2,_record) == true){

			return true;
		}
	}
	else if(L_Mex == true && R_Mex == true){ //both out


		if(notagain == false){
			//cout<< "BOTH OUT MEM>>>>>>>>>>>>>>>>>>>>>>"<<endl;
			notagain = true;
		}

		if(left_sublist_count > 0 && right_sublist_count > 0){

			//cout<<"schemaLeft"<<endl;

			if(TOBE ==false){
				//cout<< "Create_col>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
				Create_col(left_sublist_count,right_sublist_count);



				TOBE = true;
			}


	GET_NEXT_MINS:
			if(leftbucket.size() > 0 && rightbucket.size() > 0 && JoinReady== false ){ // the column are ready
				//cout<<"left SIZE"<<endl;
				//whichTableOP 0: when the left is the min table, when caled it fill left min with mins and l_chunck with potential min
				//whichTableOP 1: this vice versa
				//whichTableOP 2: when left_min > right_min
				//whichTableOP 3: when left_min < right_min





				multimap<Record,string>::iterator _it;
				abs_left_min  = (*leftbucket.begin()).first; // get the smallest value

				std::vector<string> replacements;

				Record Z =(*rightbucket.begin()).first;
				if(abs_left_min.IsEqual( Z) == true ){
					// PROCEED as planned
					abs_right_min =(*rightbucket.begin()).first;
				}
				else if(abs_left_min.LessThan(Z) == true ){

					// get new left L < R;
					multimap<Record,string>::iterator _it;
					for(multimap<Record,string>::iterator it =leftbucket.begin(); it != leftbucket.end(); ){
						Record no = (*it).first;

						if(abs_left_min.IsEqual(no) == true){
							//left_mins.push_back((*it).first);
							replacements.push_back((*it).second);
							//_it = it;
						}else{
							//thier not equal so it GreaterThan

							break;
						}

						leftbucket.erase(it++);
					}
					for (int i = 0; i< replacements.size(); i++) { // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i], 0, abs_left_min);
					}
					replacements.clear();

					goto GET_NEXT_MINS;
				}
				else{
					//L > R;
					Record current_R_min = (*rightbucket.begin()).first;

					for(multimap<Record,string>::iterator it =rightbucket.begin(); it != rightbucket.end(); ){
						Record no = (*it).first;

						if(current_R_min.IsEqual(no) == true){
							//left_mins.push_back((*it).first);
							replacements.push_back((*it).second);
							//_it = it;
						}else{
							//thier not equal so it GreaterThan

							break;
						}


						rightbucket.erase(it++);


					}
					for (int i = 0; i< replacements.size(); i++) { // get all the min from the index of the previous in l_chunks (were)

						GetNextRow(replacements[i], 1, current_R_min);
					}
					replacements.clear();

					goto GET_NEXT_MINS;

				}


				// GET ALL MIN FROM RIGHT
				/*
				for (multimap<Record, string>::iterator it = rightbucket.begin(); it != rightbucket.end(); ) {
					Record no = (*it).first;
					no.print(cout, schemaRight);
					cout << endl;
					it++;
				}*/

				for(multimap<Record,string>::iterator it = rightbucket.begin(); it != rightbucket.end(); ){
					Record no = (*it).first;

					if(abs_right_min.IsEqual(no)){
						right_mins.push_back((*it).first);
						replacements.push_back((*it).second);


					}else{
						//thier not equal so it GreaterThan

						break;
					}
					rightbucket.erase(it++);
				}

				for (int i = 0; i< replacements.size(); i++) { // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i], 2, abs_right_min);

				}
				replacements.clear();



				/*
				for (multimap<Record, string>::iterator it = rightbucket.begin(); it != rightbucket.end(); ) {
					Record no = (*it).first;
					no.print(cout, schemaRight);
					cout << endl;
					it++;
				}*/

				//GET ALL MIN FROM LEFT

				for(multimap<Record,string>::iterator it = leftbucket.begin(); it != leftbucket.end(); ){
					Record no = (*it).first;

					if(abs_left_min.IsEqual(no)){
						left_mins.push_back((*it).first);
						replacements.push_back((*it).second);
						_it =it;
					}else{
						//thier not equal so it GreaterThan

						break;
					}

					++it;
					leftbucket.erase(_it);
				}
				for (int i = 0; i< replacements.size(); i++) { // get all the min from the index of the previous in l_chunks (were)

					GetNextRow(replacements[i], 3, abs_left_min);

				}
				replacements.clear();
				//start Join
				if(left_mins.size() > 0 && right_mins.size() > 0){ // i got some min lets join
					JoinReady = true;

					left_mins_it = left_mins.begin();
					right_mins_it = right_mins.begin();
				}

			}

			//ONE TO ALL JOIN ON THE MINS
			if(JoinReady == true){

				while( right_mins_it != right_mins.end() ){

					while( left_mins_it != left_mins.end() ){

						//cout << "CHECKING JOIN" << endl;
						Record right= (*right_mins_it);
						Record left =(*left_mins_it);

						//right.print(cout, schemaRight);
					//	cout << endl;
					//	left.print(cout, schemaLeft);
					//	cout << endl;

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
				/*
				cout << leftbucket.size() << endl;
				cout << rightbucket.size() << endl;

				if (!rightbucket.empty()) {
					cout << " ITS NOT EMPTY" << endl;
					for (multimap<Record, string>::iterator it = rightbucket.begin(); it != rightbucket.end(); ) {
						Record no = (*it).first;
						no.print(cout, schemaRight);
						cout << endl;
						it++;

					}
				}*/

				goto GET_NEXT_MINS;


			}

        }
	}


	return false;
}

void Join::GetNextRow(string index, int whichTableOP, Record& min){

	//which table 0=left, 1=right

	Record next;

	if( whichTableOP ==0){  // replace bad min for Left side L < R

		for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {

			if(index != (*it).GetName() ){

				continue;
			}else{
		//		(*it).MoveFirst();
				while ((*it).GetNext(next) == true) {

					next.SetOrderMaker(_omL);
					int result = _omL->Run2ElectricBoogaloo(min,next,*_omL);
					if(result == 0){
						//left_mins.push_back(next);
						continue;
					}
					else if(result == -1){  //min is less than next rec

						//cout << "INSERTING LEFT1" << endl;
						leftbucket.insert(pair<Record,string>( next,(*it).GetName()) );
						return;
					}
				}
			}
		}
	}
	if(whichTableOP == 1){ // replace bad min for the right side L > R

		for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {

			if(index != (*it).GetName() ){

				continue;
			}else{
			//	(*it).MoveFirst();
				while((*it).GetNext(next) == true) {

					next.SetOrderMaker(_omR);

					int result = _omR->Run2ElectricBoogaloo(min,next,*_omR);
					if(result == 0){
						// we dont want it smaller than left
						continue;
					}
					else if(result == -1){  //min is less than next rec

						//cout << "INSERTING RIGHT1" << endl;
						rightbucket.insert(pair<Record,string>(next,index));
						return;
					}
				}
			}
		}
	}

	if(whichTableOP == 2){ // get Next on all good min

		for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {

			if(index != (*it).GetName() ){

				continue;
			}else{
			//	(*it).MoveFirst();
				while((*it).GetNext(next) == true) {

					next.SetOrderMaker(_omR);

					int result = _omR->Run2ElectricBoogaloo(min,next,*_omR);
					if(result == 0){
						right_mins.push_back(next);

					}
					else if(result == -1){  //min is less than next rec

						rightbucket.insert(pair<Record,string>(next,index));
						return;
					}
				}
			}
		}
	}

	if( whichTableOP ==3){  // replace bad min for Left side L < R

		for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {

			if(index != (*it).GetName() ){

				continue;
			}else{
			//	(*it).MoveFirst();
				while ((*it).GetNext(next) == true) {

					next.SetOrderMaker(_omL);
					int result = _omL->Run2ElectricBoogaloo(min,next,*_omL);
					if(result == 0){
						left_mins.push_back(next);

					}
					else if(result == -1){  //min is less than next rec

						leftbucket.insert(pair<Record,string>( next,(*it).GetName()) );
						return;
					}
				}
			}
		}
	}

}



Join::~Join(){

	delete _omR;
	delete _omL;
	_omL = NULL;
	_omR = NULL;


	for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {

		(*it).Close();

		cout<<"Closing Files"<<endl;
	}


	for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {
		(*it).Close();

		cout<<"Closing Files"<<endl;

	}

}


bool Join::RUNNER(int option,Record& _record){
	Record temp, Rec;
	multimap<Record, int>::iterator _it;


	if(option == 0){
		if(leftSmaller){

			if( R_it != Rrec.end() ){

				temp = (*R_it).first;

				multimap<Record, int>::iterator it = Lrec.find(temp);
					//Couldn't find record in the map, advance until A found
				while(it == Lrec.end()){

					R_it++;

					if( R_it != Rrec.end() ){

						temp = (*R_it).first;
						it = Lrec.find(temp);
					}
					else{
						//reached the end
						return false;
					}
				}

				Rec = (*it).first;


				if( R_it != Rrec.end() ){
					//cout<<"hey"<<endl;
					R_it++;
				}

				if(predicate.Run(Rec, temp) == true){

					_record.AppendRecords(Rec, temp, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());

					return true;
				}

			}


		}else{

			if(L_it != Lrec.end()){

				temp = (*L_it).first;

				multimap<Record, int>::iterator it= Rrec.find(temp);

				 //Couldn't find record in the map, advance until A found
				while(it == Rrec.end()){

					L_it++;

					if(L_it != Lrec.end() ){

						temp = (*L_it).first;
						it = Rrec.find(temp);
					}
					else{
						return false;//reached the end
					}
				}

				Rec = (*it).first;

				if( L_it != Lrec.end() ){
					L_it++;
				}

				if(predicate.Run(temp, Rec)){
					Record T = temp;
					_record.AppendRecords(T, Rec, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());

					return true;
				}
			}
		}
	}


	if(option == 1){
		//else if(L_Mex == true && R_Mex == false){

		while(V_L_it != L_files.end() ){

			Record bigleft;
			Record litright;
			while((*V_L_it).GetNext(bigleft)){
				//cout<<TOBE++<<endl;
				bigleft.SetOrderMaker(_omL);

				_it = Rrec.find(bigleft);

				if(_it != Rrec.end() ){

					for (std::multimap<Record, int>::iterator it = _it; it != Rrec.end(); ++it){
						litright = (*_it).first;

						if(predicate.Run(bigleft, litright) == true){

							_record.AppendRecords(bigleft, litright, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());
							//last_file_accessed = (*it).GetName();
							return true;
						}
					}


				}
			}

			if( (*V_L_it).GetNext(bigleft) == false) {
				V_L_it++;
				continue;
			}

		}
	}

	if(option == 2){
		//L_Mex == false && R_Mex == true

		while(V_R_it != R_files.end() ){

			Record litleft;
			Record bigright;

			while((*V_R_it).GetNext(bigright)){

				bigright.SetOrderMaker(_omR);

				_it = Lrec.find(bigright);

				if(_it != Lrec.end() ){

					for (std::multimap<Record, int>::iterator it = _it; it != Lrec.end(); ++it){
						litleft = (*_it).first;

						if(predicate.Run(litleft,bigright) == true){

							_record.AppendRecords(litleft,bigright, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());

							return true;
						}
					}
				}
			}

			if( (*V_R_it).GetNext(bigright) == false) {
				V_R_it++;
				continue;
			}

		}
	}


	return false;
}


void Join::Create_col(int left_size, int right_size){
	if (chunks_filled == false) {

		if(left_size <= right_size){

			for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {
				Record temp_rec;
				L_PageSizeInBytes = 0;
				(*it).MoveFirst();
				while ((*it).GetNext(temp_rec)) {

					char* b = temp_rec.GetBits();
					temp_rec.SetOrderMaker(_omL);
					leftbucket.insert(pair<Record,string>(temp_rec,(*it).GetName() ));
					L_PageSizeInBytes += ((int*)b)[0];

					break;
				}
			}

			for (vector<DBFile>::iterator _it = R_files.begin(); _it != R_files.end(); _it++) {
				//cout << "WE REACH HERE" << endl;
				Record temp_rec;

				R_PageSizeInBytes = 0;
				(*_it).MoveFirst();
				while ((*_it).GetNext(temp_rec)) {

					char* b = temp_rec.GetBits();
					temp_rec.SetOrderMaker(_omR);
					rightbucket.insert(pair<Record,string> (temp_rec,(*_it).GetName()));
					R_PageSizeInBytes += ((int*)b)[0];

					break;
				}
			}

		}else{

			for (vector<DBFile>::iterator _it = R_files.begin(); _it != R_files.end(); _it++) {
				//cout << "WE REACH HERE" << endl;
				Record temp_rec;

				R_PageSizeInBytes = 0;
				(*_it).MoveFirst();
				while ((*_it).GetNext(temp_rec)) {

					char* b = temp_rec.GetBits();
					temp_rec.SetOrderMaker(_omR);
					rightbucket.insert(pair<Record,string> (temp_rec,(*_it).GetName()));
					R_PageSizeInBytes += ((int*)b)[0];

					break;

				}
			}
			for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {
				Record temp_rec;
				L_PageSizeInBytes = 0;
				(*it).MoveFirst();
				while ((*it).GetNext(temp_rec)) {
					char* b = temp_rec.GetBits();
					temp_rec.SetOrderMaker(_omL);
					leftbucket.insert(pair<Record,string>(temp_rec,(*it).GetName() ));
					L_PageSizeInBytes += ((int*)b)[0];
					break;

				}
			}
		}
		chunks_filled = true;
	}
}



ostream& Join::print(ostream& _os) {
	//string joined = StringStrem(*left);
	//string a = schemaOut;
	//vector<int> removeLeft;
	Schema tempRight = schemaRight;
	Schema tempLeft = schemaLeft;

	//	cout << tempRight << endl;
	//	cout << tempLeft << endl;
	//	cout << "COMPARE NUMBERS " << _predicate.numAnds << endl;

	vector<int> removeLeft;
	for (int i = 0; i < predicate.numAnds; i++) {
		removeLeft.push_back(predicate.andList[i].whichAtt1);
	}


	vector<int> removeRight;
	for (int j = 0; j < predicate.numAnds; j++) {
		removeRight.push_back(predicate.andList[j].whichAtt2);
	}


	//cout << "LEFT" << endl;
	//	cout << tempLeft << endl;
	//	cout << "RIGHT" << endl;
	//		cout << tempRight << endl;
	tempRight.Project(removeRight);
	//	cout <<"PROJECTED" <<tempRight << endl;
	tempLeft.Project(removeLeft);

	vector<int> removeAll;
	for (int j = 0; j < predicate.numAnds; j++) {
		removeRight.push_back(predicate.andList[j].whichAtt2);
		removeRight.push_back(predicate.andList[j].whichAtt1);
		//	cout << predicate.andList[j].whichAtt1 << " ";
		//	cout << predicate.andList[j].whichAtt2 << endl;
	}

	Schema OUT = schemaOut;
	//cout << OUT << endl;
	OUT.Project(removeAll);

	Schema tempOut = tempLeft;
	//schemaOut = tempLeft;
	tempOut.Append(tempRight);

	cout << tempOut << endl;
	vector<Attribute> a;
	a = tempOut.GetAtts();
	string s;
	for (int i = 0; i < a.size(); i++) {
		s += a[i].name;
		s += " ";
	}

	string tabs = "";
	for (int i = 0; i < tab; i++) {
		tabs += "\t";
	}
	tab++;
	cout << tabs;




	if (Select* a = dynamic_cast<Select*>(left))
		return _os << "JOIN" << tempOut << " RIGHT CHILD " << *right << endl << "\t" << tabs << *left;
	if (Scan* a = dynamic_cast<Scan*>(left))
		return _os << "JOIN" << tempOut << " RIGHT CHILD " << *right << endl << "\t" << tabs << *left;
	else
		return _os << "JOIN" << tempOut << " RIGHT CHILD " << *right << endl << "\t" << *left;
}

DuplicateRemoval::DuplicateRemoval(Schema& _schema, RelationalOp* _producer) {
	schema = _schema;
	producer = _producer;

}

DuplicateRemoval::~DuplicateRemoval() {

}

bool DuplicateRemoval::GetNext(Record& _record)
{
	//Record temp;
	//check to see if next isThere
	bool exists = producer->GetNext(_record);

	//intialize below
	char* recBits;
	int recSize = 0;
	string someString;
	KeyInt temper = 0;

	//cout << "CRASHED 1" << endl;
	while (exists)
	{

		//gets char* and int size below
		recBits = _record.GetBits();
		recSize = _record.GetSize();

		someString.resize(recSize); // make sure we have enough space!
		memcpy(&someString[0], recBits, recSize);

		//below is hash "funtion"
		int hashRec = 0;
		int offset = 'a' - 1;

		for (string::const_iterator it = someString.begin(); it != someString.end(); ++it)
		{

			hashRec = hashRec << 1 | (*it - offset);

		}


		if (hashRec < 0) {
			hashRec = -hashRec;
		}

		stringstream ss;

		_record.print(ss, schema);

		//	cout << "STRING" <<ss.str() << endl;

		KeyInt hashKey = hashRec;

		//	KeyInt hashKey;

		KeyString hashString = ss.str();

		if (treasureMap2.Length() == 0) {

			treasureMap2.Insert(hashString, temper);
			//cout << "FIRST INSERT" << endl;
			return true;
		}
		else if (treasureMap2.IsThere(hashString) != 1)
		{
			//if it's in the map, just get the next one


			//we got a distinct

			//add hashRec and _record to treasureMap
			treasureMap2.Insert(hashString, temper);
			//_record = temp;
			return true;

		}

		else
		{
			//cout << "CRASHED 3" << endl;
			exists = producer->GetNext(_record);
		}
		/*
		if (treasureMap.Length() == 0) {

		treasureMap.Insert(hashKey, temper);
		//cout << "FIRST INSERT" << endl;
		return true;
		}
		else if (treasureMap.IsThere(hashKey) != 1)
		{
		//if it's in the map, just get the next one


		//we got a distinct

		//add hashRec and _record to treasureMap
		treasureMap.Insert(hashKey, temper);
		//_record = temp;
		return true;

		}

		else
		{
		//cout << "CRASHED 3" << endl;
		exists = producer->GetNext(_record);
		}*/
		//exists = producer->GetNext(_record);
	}
	return false;
}


ostream& DuplicateRemoval::print(ostream& _os) {
	return _os << "DISTINCT " << *producer;
}



Sum::Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
	RelationalOp* _producer) {

	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	compute = _compute;
	producer = _producer;



}

Sum::~Sum() {

}

bool Sum::GetNext(Record& _record) {

	//cout << "GETNEXTSUM" << endl;

	bool isThereMore;
	bool sumIsAFloat = 0;	// this is terrible and i feel bad
	int intSum = 0;
	double floatSum = 0;
	int intResult;
	double doubleResult;

	Record temp;

	int* tempBits;

	isThereMore = producer->GetNext(temp);
	if ((isThereMore) == false)
		return false;

	while (isThereMore) {		 // if there is more, keep going

		intResult = 0;
		doubleResult = 0.0;
		//	cout << compute.Apply(temp, intResult, doubleResult) << endl;
		if (compute.Apply(temp, intResult, doubleResult) == 0) {	// it returned an int, ignore doubleResult


			intSum += intResult;

			//	cout << intSum << " " << intResult<< endl;

		}
		else {	// it returned a float
			sumIsAFloat = 1;
			floatSum += doubleResult;

			//	cout << " FLOAT" << floatSum << "doubleResult " << doubleResult <<" intResult " << intResult <<endl;

		}

		isThereMore = producer->GetNext(temp);	// keep going until you run out of producer
	}

	//	cout << "RESULT FLOAT" << floatSum << endl;
	// now that you have your sum...
	// ----------- void Record :: CopyBits(char* _bits, int b_len)

	if (!sumIsAFloat) {
		// use intSum
		tempBits = new int[3];
		tempBits[0] = sizeof(int) * 3;	// size of rec
		tempBits[1] = sizeof(int) * 2;	// location of attribute 1 start
		tempBits[2] = intSum;	// simple, since an int
								// stick into _record
		_record.Nullify();
		_record.CopyBits((char*)tempBits, tempBits[0]);
	}
	else {
		// use floatSum
		cout << "HERE" << endl;
		tempBits = new int[4];
		tempBits[0] = sizeof(int) * 4;	// size of rec
		tempBits[1] = sizeof(int) * 2;	// location of attribute 1 start
		memcpy(&tempBits[2], &floatSum, sizeof(double));	// less simple, since a float, but still just memcpy
															// stick into _record
		_record.Nullify();
		_record.CopyBits((char*)tempBits, tempBits[0]);
	}


	delete[] tempBits;
	return true;
	/*
	int intSum = 0;
	double floatSum = 0;
	int intResult;
	double doubleResult;

	Record temp;
	while (producer->GetNext(temp)) {
	intResult = 0;
	doubleResult = 0;
	compute.Apply(temp, intResult, doubleResult);
	intSum += intResult;
	floatSum += doubleResult;
	cout << " FLOAT " << floatSum << "doubleResult " << doubleResult << " intResult " << intResult << endl;

	}*/

	return true;

}

ostream& Sum::print(ostream& _os) {
	vector<Attribute> a;
	a = schemaOut.GetAtts();
	string s;
	for (int i = 0; i < a.size(); i++) {
		s += a[i].name;
		s += " ";
	}
	//compute

	return _os << "SUM( " << s << ")" << endl << "\t" << *producer;
}


GroupBy::GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
	Function& _compute, RelationalOp* _producer) {

	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	groupingAtts.Swap(_groupingAtts);
	compute = _compute;
	producer = _producer;




}

GroupBy::~GroupBy() {

}

bool GroupBy::GetNext(Record& _record) {





	bool isThereMore;
	int intResult;
	double doubleResult;

	Record _sumRec;
	Record _attRec;

	cout << schemaOut << endl;
	vector<int> tempGroup;

	Schema inGroup = schemaOut;
	for (int i = 1; i < schemaOut.GetNumAtts(); i++) {
		tempGroup.push_back(i);
	}
	inGroup.Project(tempGroup);
	cout << inGroup << endl;

	cout << "GROUP BY" << endl;
	isThereMore = producer->GetNext(_record);

	int* recBits;
	int recSize = 0;
	string someString;
	int tempRecInt;

	KeyString recString;
	KeyDouble recDouble;
	KeyInt recInt;

	//these are for removing from map
	KeyString randoKey;
	KeyInt randoIntData;
	KeyDouble randoDoubData;

	if (!ran) {


		if (isThereMore == false) {
			return false;
		}

		while (isThereMore) {

			//_record.print(cout, schemaIn);

			// THIS SECTION CREATES "HASHKEY" - concatenated version of VALUES of all grouping atts

			KeyString hashString;
			string tempStr = "";

			//int numAtts = groupingAtts.numAtts;
			//groupingAtts(schemaOut);
			//int numAtts = schemaOut.GetNumAtts()-1;
			Schema inGroup = schemaOut;
			for (int i = 1; i < schemaOut.GetNumAtts(); i++) {
				tempGroup.push_back(i);
			}
			inGroup.Project(tempGroup);

			int * keep = new int[schemaOut.GetNumAtts() - 1];
			int count = 0;

			vector<Attribute> tempIn = schemaIn.GetAtts();
			vector<Attribute> tempGroup = inGroup.GetAtts();
			for (int j = 0; j < schemaIn.GetNumAtts(); j++) {
				for (int i = 0; i<inGroup.GetNumAtts(); i++) {
					if (tempGroup[i].name == tempIn[j].name)
					{
						keep[count] = j;
						count++;
					}
				}
			}
			//	cout << "Kept"<<keep[0] << endl;

			for (int i = 0; i < groupingAtts.numAtts; i++) {
				//cout << "attribute type: " << groupingAtts.whichTypes[i] << endl;

				if (groupingAtts.whichTypes[i] == 0) {
					tempRecInt = *((int *)_record.GetColumn(keep[i]));
					//	cout << tempRecInt << endl;
					tempStr += to_string(tempRecInt);
					//	cout << endl;
					//	cout << tempStr << endl;

				}
				else if (groupingAtts.whichTypes[i] == 1) {
					double tempRecDouble = *((double *)_record.GetColumn(keep[i]));
					tempStr += to_string(tempRecInt);

				}
				else {
					//else if(groupingAtts.whichTypes[i] == 2){
					string tempRecStr = string(_record.GetColumn(keep[i]));
					//cout << tempRecStr << endl;
					tempStr += tempRecStr;

				}

			}
			//	cout <<" KEY "  <<  tempStr << endl;
			//hashString = Keyify<KeyString>(tempStr);
			hashString = tempStr;


			// now we check in Map

			AttributedDouble.MoveToStart();
			AttributedInt.MoveToStart();
			RecordMap.MoveToStart();


			RecMap.begin();

			if (compute.Apply(_record, intResult, doubleResult) == 0) {
				if (RecMap.bucket_count() == 0 || RecMap.find(tempStr) == RecMap.end()) {
					Record tempRec = _record;
					GroupRecc* inserted = new GroupRecc(_record, intResult, doubleResult);
					//	Keyify<GroupRecc*> keyed = insert;
					pair<string, GroupRecc*> insertpair(tempStr, inserted);
					//RecordMap.Insert(hashString, keyed);
					RecMap.insert(insertpair);
					cout << doubleResult << endl;

				}
				else {
					//	GroupRecc *recc = RecordMap.Find(hashString);
					auto it = RecMap.find(tempStr);
					it->second->intResult += intResult;
					//	GroupRecc *recc = RecMap.find(tempStr)->second;
					//	recc->intResult+= intResult;

				}


				//integer
				/*
				if (AttributedInt.Length() == 0 || !(AttributedInt.IsThere(hashString))) {
				//if the EfficientMap is empty or can't find hashString in map
				recInt = intResult;
				AttributedInt.Insert(hashString, recInt);


				cout << "inserted" << endl;
				}
				else if (AttributedInt.IsThere(hashString)) {
				// found in map
				int hashData = AttributedInt.Find(hashString); //returns data from map
				recInt = hashData + intResult;
				AttributedInt.CurrentData() = recInt;
				//	AttributedInt.Remove(hashString, randoKey, randoIntData);
				//	AttributedInt.Insert(hashString, recInt);

				}

				}
				else {		// double

				if (AttributedDouble.Length() == 0 || !(AttributedDouble.IsThere(hashString))) {
				//if the EfficientMap is empty or can't find hashString in map
				recDouble = doubleResult;
				AttributedDouble.Insert(hashString, recDouble);
				cout << "inserted" << endl;
				}
				else if (AttributedDouble.IsThere(hashString)) {
				// found in map
				double hashData = AttributedDouble.Find(hashString); //returns data from map
				recDouble = hashData + doubleResult;
				AttributedDouble.Remove(hashString, randoKey, randoDoubData);
				AttributedDouble.Insert(hashString, recDouble);
				//AttributedDouble.CurrentData() =
				}
				*/
			}
			else {
				if (RecMap.bucket_count() == 0 || RecMap.find(tempStr) == RecMap.end()) {
					Record tempRec = _record;
					GroupRecc* inserted = new GroupRecc(_record, intResult, doubleResult);
					//	Keyify<GroupRecc*> keyed = insert;
					pair<string, GroupRecc*> insertpair(tempStr, inserted);
					//RecordMap.Insert(hashString, keyed);
					RecMap.insert(insertpair);


				}
				else {
					//	GroupRecc *recc = RecordMap.Find(hashString);
					auto it = RecMap.find(tempStr);
					it->second->doubleResult += doubleResult;

					//cout << doubleResult << endl;

					//	GroupRecc *recc = RecMap.find(tempStr)->second;
					//	recc->intResult+= intResult;

				}
			}


			//	cout << "DID WE CRASH" << endl;
			//_record.print(cout, schemaIn);

			//	_record.print(cout, schemaOut);

			isThereMore = producer->GetNext(_record);
			//	cout << "is there: " << isThereMore << endl;
			if (isThereMore == 0) {
				break;
			}
		}
		ran = true;
		//_record.print(cout, schemaOut);

	}


	it = RecMap.begin();
	Record applyRec = _record;
	while (it != RecMap.end()) {
		string att = it->first;
		GroupRecc *recc = it->second;

		it++;


		//if sum is an int
		if (compute.Apply(applyRec, intResult, doubleResult) == 0) {
			//if groupingAtt is an int
			if (groupingAtts.whichTypes[0] == 0) {
				// cout << recc->doubleResult << " " << att << endl;

				recBits = new int[5];
				recBits[0] = sizeof(int) * 5;	// size of rec
				recBits[1] = sizeof(int) * 3;	// location of attribute 1 start
				recBits[2] = sizeof(int) * 4;	// location of attribute 2 start
				recBits[3] = recc->intResult;	// Sum
				recBits[4] = stoi(att);		// attribute #

				recc->Rec.Nullify();
				recc->Rec.CopyBits((char*)recBits, recBits[0]);
				_record = recc->Rec;

			}
			// if groupingAtt is a double
			else if (groupingAtts.whichTypes[0] == 1) {
				//cout << recc->doubleResult << " " << att << endl;
				string::size_type sz;
				double tempAttDub = stod(att, &sz);

				recBits = new int[6];
				recBits[0] = (sizeof(int) * 4) + sizeof(double); //size of rec
				recBits[1] = (sizeof(int) * 3); //location of attribute 1 start
				recBits[2] = (sizeof(int) * 4); //location of attribute 2 start
				recBits[3] = recc->intResult;
				memcpy(&recBits[4], &tempAttDub, sizeof(double));

				recc->Rec.Nullify();
				recc->Rec.CopyBits((char*)recBits, recBits[0]);
				_record = recc->Rec;
			}
			// if groupingAtt is a string

			else if (groupingAtts.whichTypes[0] == 2) {
				//cout << recc->intResult << " " << att << endl;

				float stringLength = att.length() + 1;
				//cout << " stringLength " << stringLength << endl;
				int recBitsSize = (int)ceil(stringLength / sizeof(int));
				//cout << " recBitsSize " << recBitsSize << endl;
				recBitsSize += sizeof(int) + sizeof(int) * 2 + sizeof(int);
				//cout << " recBitsSize " << recBitsSize << endl;

				recBits = new int[recBitsSize];
				recBits[0] = recBitsSize; //size of rec
				recBits[1] = (sizeof(int) * 3); //location of attribute 1 start
				recBits[2] = (sizeof(int) * 3) + sizeof(int); //location of attribute 2 start
				recBits[3] = recc->intResult;
				//cout << " stringLength " << stringLength << endl;



				//cout << att << endl;
				memcpy(&recBits[4], &att[0], stringLength * sizeof(char));

				//cout<< (char*) &recBits[4] << endl;


				int j = 4;

				for (int i = 0; i < stringLength; i++) {
					//cout << att[i];
					//memcpy((&recBits[4])+i, &att[i], 1);
					if (i % 4 == 0 && i != 0) {	// if we moved one int in recBits
						j++;
					}

					memcpy((&recBits[j]) + (i % 4), &att[i], 1);

				}

				//char test = 'A';

				//memcpy((&recBits[4])+4, &test, 1);


				//}
				//cout << (char *)((&recBits[4])) ;
				//cout << endl;


				//recc->Rec.Nullify();
				//recc->Rec.CopyBits((char*)recBits, recBits[0]);
				//recc->Rec.print(cout, schemaOut);
				_record.CopyBits((char*)recBits, recBits[0] - 3 + att.length() + 3);
				//_record.print(cout, schemaOut);

			}

		}
		//if sum is a double
		else {
			// if groupingAtt is an int
			if (groupingAtts.whichTypes[0] == 0) {
				// cout << recc->doubleResult << " " << att << endl;

				recBits = new int[6];
				recBits[0] = (sizeof(int) * 4) + sizeof(double); // size of rec
				recBits[1] = (sizeof(int) * 3); // location of attribute 1 start
				recBits[2] = (sizeof(int) * 3) + sizeof(double); //location of attribute 2 start
				memcpy(&recBits[3], &recc->doubleResult, sizeof(double));
				recBits[5] = stoi(att);

				//recc->Rec.Nullify();
				//recc->Rec.CopyBits((char*)recBits, recBits[0]);
				//_record = recc->Rec;

				_record.Nullify();
				_record.CopyBits((char*)recBits, recBits[0]);


			}
			// if groupingAtt is a double
			else if (groupingAtts.whichTypes[0] == 1) {
				cout << recc->doubleResult << " " << att << endl;
				string::size_type sz;
				double tempAttDub = stod(att, &sz);

				recBits = new int[7];
				recBits[0] = (sizeof(int) * 3) + (sizeof(double) * 2); //size of rec
				recBits[1] = (sizeof(int) * 3); //location of attribute 1 start
				recBits[2] = (sizeof(int) * 3) + sizeof(double); //location of attribute 2 start
				memcpy(&recBits[3], &recc->doubleResult, sizeof(double)); //attribute 1
				memcpy(&recBits[5], &tempAttDub, sizeof(double)); //attribute 2

																  //recc->Rec.Nullify();
																  //recc->Rec.CopyBits((char*)recBits, recBits[0]);
																  //_record = recc->Rec;

				_record.Nullify();
				_record.CopyBits((char*)recBits, recBits[0]);
				// cout << "Here" << endl;
				//_record.print(cout, schemaOut);
				// cout << endl;
			}
			// if groupingAtt is a string, and I DON'T KNOW
			else if (groupingAtts.whichTypes[0] == 2) {
				//cout << recc->intResult << " " << att << endl;

				float stringLength = att.length() + 1;
				//cout << " stringLength " << stringLength << endl;
				int recBitsSize = (int)ceil(stringLength / sizeof(int));
				//cout << " recBitsSize " << recBitsSize << endl;
				recBitsSize += sizeof(int) + sizeof(int) * 2 + sizeof(double);
				//cout << " recBitsSize " << recBitsSize << endl;

				recBits = new int[recBitsSize];
				recBits[0] = recBitsSize; //size of rec
				recBits[1] = (sizeof(int) * 3); //location of attribute 1 start
				recBits[2] = (sizeof(int) * 3) + sizeof(double); //location of attribute 2 start
																 //recBits[3] = recc->doubleResult;

				memcpy(&recBits[3], &(recc->doubleResult), sizeof(double));
				//cout << " stringLength " << stringLength << endl;



				//cout << att << endl;
				memcpy(&recBits[5], &att[0], stringLength * sizeof(char));

				//cout<< (char*) &recBits[5] << endl;

				int j = 5;

				for (int i = 0; i < stringLength; i++) {
					//cout << att[i];
					//memcpy((&recBits[4])+i, &att[i], 1);
					if (i % 4 == 0 && i != 0) {	// if we moved one int in recBits
						j++;
					}

					memcpy((&recBits[j]) + (i % 4), &att[i], 1);

				}



				//}
				//cout << (char *)((&recBits[5])) ;
				//cout << endl;


				//recc->Rec.Nullify();
				//recc->Rec.CopyBits((char*)recBits, recBits[0]);
				//recc->Rec.print(cout, schemaOut);
				_record.CopyBits((char*)recBits, recBits[0] - 3 + att.length() + 3);
				//_record.print(cout, schemaOut);

			}
			/*else if(groupingAtts.whichTypes[0] == 2){
			cout << recc->doubleResult << " " << att << endl;

			float stringLength = sizeof(att);
			int recBitsSize = (int) ceil(stringLength/sizeof(int));
			recBitsSize += sizeof(int) + sizeof(int)*2 + sizeof(double);

			recBits = new int[recBitsSize];
			recBits[0] = recBitsSize; //size of rec
			recBits[1] = (sizeof(int) * 3); //location of attribute 1 start
			recBits[2] = (sizeof(int) * 3) + sizeof(double); //location of attribute 2 start
			//recBits[3] = recc->doubleResult;

			memcpy(&recBits[3], &recc->doubleResult, sizeof(double));
			memcpy(&recBits[4], &att, stringLength);

			recc->Rec.Nullify();
			recc->Rec.CopyBits((char*)recBits, recBits[0]);
			_record = recc->Rec;
			}*/

		}
		// if sum is a double

		// if sum is a string



		_record.print(cout, schemaOut);
		//return true;
		cout << endl;
		//return spitRecord(_record);

	}

	/*
	RecordMap.MoveToStart();
	while (!RecordMap.AtEnd()) {

	GroupRecc *recc = RecordMap.CurrentData();

	recBits = new int[3];
	recBits[0] = sizeof(int) * 3;	// size of rec
	recBits[1] = sizeof(int) * 2;	// location of attribute 1 start
	recBits[2] = recc->intResult;
	recc->Rec.Nullify();
	recc->Rec.CopyBits((char*)recBits, recBits[0]);

	_record = recc->Rec;
	return true;

	RecordMap.Advance();
	}*/


	/*
	if (AttributedDouble.Length() == 0) {
	cout << "BREAKED" << endl;
	// nothing in double; that means Sum was an int
	Record temp;


	AttributedInt.MoveToStart();

	while (!AttributedInt.AtEnd()) {
	cout << "MAP" << endl;
	int _sumData = AttributedInt.CurrentData();
	KeyString removeThis = AttributedInt.CurrentKey();

	// 	Record _sumRec;
	//	Record _attRec;

	cout << "KEY " << removeThis << " SUM " << _sumData << endl;


	string tempString = removeThis;
	string token;
	vector<string> GroupKeys;
	//	string hellWrld = tempString.replaceAll("FILLER", "~");
	/*
	regex pattern("FILLER");
	string seperated = regex_replace(tempString, pattern, "~");
	istringstream ss(seperated);
	while (getline(ss, token, '~')) {
	//cout << token << '\n';
	string remove = token;
	GroupKeys.push_back(remove);
	}*/

	/*


	recBits = new int[3];
	recBits[0] = sizeof(int) * 3;	// size of rec
	recBits[1] = sizeof(int) * 2;	// location of attribute 1 start
	recBits[2] = _sumData;	// simple, since an int
	// stick into _record
	//_sumRec.Nullify();
	//_sumRec.CopyBits((char*)recBits, recBits[0]);
	_record.Nullify();
	_record.CopyBits((char*)recBits, recBits[0]);

	*//*
	Schema inGroup = schemaOut;
	for (int i = 1; i < schemaOut.GetNumAtts(); i++) {
	tempGroup.push_back(i);
	}
	inGroup.Project(tempGroup);

	int * keep = new int [schemaOut.GetNumAtts() - 1];
	int count = 0;

	vector<Attribute> tempIn = schemaIn.GetAtts();
	vector<Attribute> tempGroup = inGroup.GetAtts();
	for (int j = 0; j < schemaIn.GetNumAtts(); j++) {
	for (int i = 0; i<inGroup.GetNumAtts(); i++) {
	if (tempGroup[i].name == tempIn[j].name)
	{
	keep[count] = j;
	count++;
	}
	}
	}*/

	/*
	AttributedInt.Remove(removeThis, randoKey, randoIntData);
	//	AttributedInt.Advance();
	return true;
	*/

	return false;
	//}
	//else {		// Sum was a double

	//}




}


bool GroupBy::spitRecord(Record &_record) {
	if (_record.GetSize() != 0) {
		return true;
	}
	else
		return false;
}




ostream& GroupBy::print(ostream& _os) {
	vector<Attribute> a;
	a = schemaOut.GetAtts();
	string s;
	for (int i = 1; i < a.size(); i++) {
		s += a[i].name;
		s += " ";
	}

	return _os << "GROUP BY " << s << " ," << *producer;
}


WriteOut::WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer) {

	schema = _schema;
	outFile = _outFile;
	producer = _producer;
}

WriteOut::~WriteOut() {

}

bool WriteOut::GetNext(Record& _record) {

	//cout << "called WRITE OTU GETNEXt" << endl;

	while (producer->GetNext(_record)) {
		//	cout << "BEFORE GETBITS" << endl;
		if (_record.GetBits() != 0) {

			//	cout << "WHY"<<_record.GetSize() << endl;

			_record.print(cout, schema);
			cout << endl;
		}
		//	if(_record.GetSize())
		//	cout << _record.GetSize() << endl;
		//cout << "WRITE OUT TRUE" << endl;
		return true;
	}


	cout << "write out print returns false" << endl;
	return false;

}


ostream& WriteOut::print(ostream& _os) {

	//return _os << "OUTPUT" << endl <<"\t" <<*producer;
	/*Record rec0;

	while(GetNext(rec0))
	{
	cout << schema << endl;
	cout << rec0.GetBits() << endl;
	//_os = _os + rec0;
	//_os.Append(rec0);
	//cout << schema << endl;
	rec0.print(_os, schema);
	}
	*/

	return _os << "OUTPUT" << endl << "\t" << *producer;

}


void QueryExecutionTree::SetMem(unsigned int p) {
	QPage = p;
	Pages = p;
}

void QueryExecutionTree::ExecuteQuery() {
	Record a;
	cout << "STARTING QUERY" << endl;
	while (root->GetNext(a)) {

	}
}

ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	tab = 0;
	return _os << "QUERY EXECUTION TREE" << endl << *_op.root;
}


GroupRecc::GroupRecc(Record recc, int intR, double doubleR) {
	Rec = recc;
	intResult = intR;
	doubleResult = doubleR;
}

GroupRecc::~GroupRecc()
{
}



	//bool didweadd = false;

	//	cout << "FIRST MAP" << endl;
/*
	for (vector<DBFile>::iterator it = R_files.begin(); it != R_files.end(); it++) {
		Record temp_rec;
		//(*it).MoveFirst();
		if((*it).GetNext(temp_rec)){
	//	while ((*it).GetNext(temp_rec)) {
	//	if (didweadd == false) {
			temp_rec.SetOrderMaker(_omR);

			Rrec2.insert(pair<Record, int>(temp_rec, count));
			//temp.print(cout, schemaRight);
			count++;

		//	temp_rec.print(cout, schemaRight);
		//	cout << endl;
		//}
		}
		//Rrec2.insert(pair<Record, int>(temp_rec, count));
		//count++;

		cout << "HOW MANY" << endl;
	}
	cout << Rrec2.size() << endl;
	for (multimap<Record, int>::iterator it = Rrec2.begin(); it != Rrec2.end(); it++) {
		Record temp = ((*it).first);
		temp.print(cout,schemaRight);
		cout << endl;
	}

	count = 0;
	cout << "SECOND MAP" << endl;
	for (vector<DBFile>::iterator it = L_files.begin(); it != L_files.end(); it++) {
		Record temp_rec;
		(*it).GetNext(temp_rec);
		temp_rec.SetOrderMaker(_omL);
		Lrec2.insert(pair<Record, int>(temp_rec, count));
		count++;

	}

	for (multimap<Record, int>::iterator it = Lrec2.begin(); it != Lrec2.end(); it++) {
		Record temp = ((*it).first);
		temp.print(cout, schemaLeft);
		cout << endl;
	}



	cout << "IT MADE MAP" << endl;;
	return false;*/


	//	return false;
	//	cout << "CHUNKS" << endl;
/*
	while (!l_chunks.empty() && !r_chunks.empty()) {

		Record left_min = l_chunks[0][0];
		Record right_min = r_chunks[0][0];
		int left_min_index = 0;
		int right_min_index = 0;
		//left_min.print(cout, schemaLeft);
		//right_min.print(cout, schemaRight);

		for (int i = 0; i < l_chunks.size(); i++) {

			int type = _omL->Run2ElectricBoogaloo(l_chunks[i][0], left_min, *_omL);

			if (type == -1 || type == 0) {
				left_min = l_chunks[i][0];
				left_min_index = i;
			}

		}

		for (int i = 0; i < r_chunks.size(); i++) {

			int type = _omR->Run2ElectricBoogaloo(r_chunks[i][0], right_min, *_omR);
			if (type == -1 || type == 0) {
				right_min = r_chunks[i][0];
				right_min_index = i;
			}
		}

		int order_maker_result = _omL->Run2ElectricBoogaloo(left_min, right_min, *_omR);

		if (order_maker_result == -1) { // if left_min < right_min
										//	cout << "ORDER -1" << endl;
			l_chunks[left_min_index].erase(l_chunks[left_min_index].begin());
			if (l_chunks[left_min_index].empty()) {
				vector<Record> temp_vec;
				Record temp_rec;
				chunk_size = 0;

				while (L_files[left_min_index].GetNext(temp_rec)) {
					char* b = temp_rec.GetBits();
					temp_vec.push_back(temp_rec);
					chunk_size += ((int*)b)[0];

					if (chunk_size + ((int *)b)[0] > max_chunk_size) { // breaks if next rec likely to go over mem constraint
						break;
					}
				}

				l_chunks[left_min_index] = temp_vec;

				// hold up
				if (l_chunks[left_min_index].empty()) {
					l_chunks.erase(l_chunks.begin() + left_min_index);

					L_files[left_min_index].Close();
					L_files.erase(L_files.begin() + left_min_index);
				}
			}
		}

		else if (order_maker_result == 1) { // if left_min > right_min
											//cout << "ORDER 1" << endl;
			r_chunks[right_min_index].erase(r_chunks[right_min_index].begin());

			if (r_chunks[right_min_index].empty()) {
				vector<Record> temp_vec;
				Record temp_rec;
				chunk_size = 0;

				while (R_files[right_min_index].GetNext(temp_rec)) {
					//temp_rec.print(cout, schemaRight);
					char* b = temp_rec.GetBits();
					temp_vec.push_back(temp_rec);
					chunk_size += ((int*)b)[0];

					if (chunk_size + ((int *)b)[0] > max_chunk_size) // breaks if next rec likely to go over mem constraint
						break;
				}

				r_chunks[right_min_index] = temp_vec;

				if (r_chunks[right_min_index].empty()) {
					r_chunks.erase(r_chunks.begin() + right_min_index);

					R_files[right_min_index].Close();
					R_files.erase(R_files.begin() + right_min_index);
				}
			}
		}
		else if (order_maker_result == 0) { // if left_min == right_min
											// if left table is larger:
											//	cout << "ORDER 0" << endl;
			if (!leftSmaller) {
				l_chunks[left_min_index].erase(l_chunks[left_min_index].begin());
				if (l_chunks[left_min_index].empty()) {
					vector<Record> temp_vec;
					Record temp_rec;
					chunk_size = 0;

					while (L_files[left_min_index].GetNext(temp_rec)) {
						char* b = temp_rec.GetBits();
						temp_vec.push_back(temp_rec);
						chunk_size += ((int*)b)[0];

						if (chunk_size + ((int *)b)[0] > max_chunk_size) {
							break;
						}
					}

					l_chunks[left_min_index] = temp_vec;

					// hold up
					if (l_chunks[left_min_index].empty()) {
						l_chunks.erase(l_chunks.begin() + left_min_index);

						L_files[left_min_index].Close();
						L_files.erase(L_files.begin() + left_min_index);
					}
				}
			}
			else if (leftSmaller) {
				r_chunks[right_min_index].erase(r_chunks[right_min_index].begin());
				if (r_chunks[right_min_index].empty()) {
					vector<Record> temp_vec;
					Record temp_rec;
					chunk_size = 0;

					while (R_files[right_min_index].GetNext(temp_rec)) {
						char* b = temp_rec.GetBits();
						temp_vec.push_back(temp_rec);
						chunk_size += ((int*)b)[0];

						if (chunk_size + ((int *)b)[0] > max_chunk_size) {
							break;
						}
					}

					r_chunks[right_min_index] = temp_vec;

					// hold up
					if (r_chunks[right_min_index].empty()) {
						r_chunks.erase(r_chunks.begin() + right_min_index);

						R_files[right_min_index].Close();
						R_files.erase(R_files.begin() + right_min_index);
					}
				}
			}

			// Still need to merge and join, I guess you just append and then return _record?

			//cout << "CHECKING JOIN" << endl;
			if (predicate.Run(left_min, right_min) == true) {
				_record.AppendRecords(left_min, right_min, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());
				return true;
			}

		}
	}

*/


/*
Record temp, Rec;
int left_sublist_count = 0;
int right_sublist_count = 0;
stringstream ss;

if(join == false){

//this check to see how has the highest amount of comment
if(schemaRight.GetMaxTuples(schemaRight)  >= schemaLeft.GetMaxTuples(schemaLeft){
leftSmaller = true;
}

while(left->GetNext(temp)){

char* b = temp.GetBits();
if( L_PageSizeInBytes + ((int *) b)[0] > (3* PAGE_SIZE)){ // MeM cap reached store to disk

//store to disk
DBFile myFile;
myFile = Blank;

ss << left_sublist_count;

string filePathS = "SortedFile/lRun" + ss.str() + ".sorted";
left_sublist_count++;

myFile.Create(const_cast<char*>(filePathS.c_str()), FileType::Sorted);
myFile.Open(filePathS.c_str());

for (std::multimap<char,int>::iterator it=Lrec.begin(); it!=Lrec.end(); ++it){
Record ToAdd = (*it).first;
myFile.AppendRecord(ToAdd);

}
L_Mex = true;
myFile.Close();
L_PageSizeInBytes = 0;
Lrec.clear();
L_files.push_back(myFile);
}

else{

L_PageSizeInBytes += ((int *) b)[0];
temp.SetOrderMaker(_omL);
Lrec.insert(pair<Record,int>(temp,0));

}

}

while(right->GetNext(temp)){
char* b = temp.GetBits();

if( R_PageSizeInBytes + ((int *) b)[0] > (3* PAGE_SIZE)){ // MeM cap reached store to disk
//store to disk
DBFile myFile;
myFile = Blank;

ss << right_sublist_count;

string filePathS = "SortedFile/rRun" + ss.str() + ".sorted";
right_sublist_count++;

myFile.Create(const_cast<char*>(filePathS.c_str()), FileType::Sorted);
myFile.Open(filePathS.c_str());

for (std::multimap<char,int>::iterator it=Rrec.begin(); it!=Rrec.end(); ++it){
Record Add = (*it).first;
myFile.AppendRecord(Add);

}
R_Mex = true;
myFile.Close();
R_PageSizeInBytes = 0;
Rrec.clear();
R_files.push_back(myFile);

}
else{

R_PageSizeInBytes += ((int *) b)[0];
temp.SetOrderMaker(_omR);
Rrec.insert(pair<Record,int>(temp,10));
}

}

if(	Rrec.size() > 0 && R_Mex == true ){
//store to disk
DBFile myFile;
myFile = Blank;

ss << right_sublist_count;

string filePathS = "SortedFile/rRun" + ss.str() + ".sorted";
right_sublist_count++;

myFile.Create(const_cast<char*>(filePathS.c_str()), FileType::Sorted);
myFile.Open(filePathS.c_str());

for (std::multimap<char,int>::iterator it=Rrec.begin(); it!=Rrec.end(); ++it){
Record Add = (*it).first;
myFile.AppendRecord(Add);

}

myFile.Close();
R_PageSizeInBytes = 0;
Rrec.clear();
R_files.push_back(myFile);

}

if( Lrec.size() > 0 && L_Mex == true  ){
//store to disk
DBFile myFile;
myFile = Blank;

ss << left_sublist_count;

string filePathS = "SortedFile/lRun" + ss.str() + ".sorted";
left_sublist_count++;

myFile.Create(const_cast<char*>(filePathS.c_str()), FileType::Sorted);
myFile.Open(filePathS.c_str());

for (std::multimap<char,int>::iterator it=Lrec.begin(); it!=Lrec.end(); ++it){
Record ToAdd = (*it).first;
myFile.AppendRecord(ToAdd);

}
L_Mex = true;
myFile.Close();
L_PageSizeInBytes = 0;
Lrec.clear();
L_files.push_back(myFile);

}

join = true;

}

//-----merge---------

if(leftSmaller){

if(right->GetNext(temp)){
temp.SetOrderMaker(_omR);

multimap<Record, int>::iterator it = MMrec.find(temp);
//Couldn't find record in the map, advance until A found
while(it == MMrec.end()){

if(right->GetNext(temp) == true){
temp.SetOrderMaker(_omR);
it = MMrec.find(temp);
}
else{
//reached the end
return false;
}
}

Rec = (*it).first;

if(predicate.Run(Rec, temp) == true){
Record T = temp;
_record.AppendRecords(Rec, temp, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());
T.Nullify();
return true;
}

}

}else{

if(left->GetNext(temp)){
temp.SetOrderMaker(_omL);

multimap<Record, int>::iterator it= MMrec.find(temp);

//Couldn't find record in the map, advance until A found
while(it == MMrec.end()){

if(left->GetNext(temp)){
temp.SetOrderMaker(_omL);
it = MMrec.find(temp);
}
else{
return false;//reached the end
}
}

Rec = (*it).first;

if(predicate.Run(temp, Rec)){
Record T = temp;
_record.AppendRecords(T, Rec, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts());

return true;
}
}

}


*/


/* ALEX.C

//int start_index = 0;
//int end_index = sublist.size();

//merge_sort(sublist, start_index, end_index, _omL)

//save to file
//empty vector

New++++++   char* b = rec.GetBits();
curSizeInBytes += ((int *) b)[0];
6
if (curSizeInBytes + ((int *) b)[0] > (Num * PAGE_SIZE))  mem exhausted
______________________________
DBFile myFile;

*** create a folder where you intend to keep the sorted file. eg :  SortFile/name_of_sortfile.Sorted
string FilePathH; // the file path to sortedFiles are

myFile.Create(const_cast<char*>(FilePathH.c_str()), FileType::Sorted)  == 0 ; sucess

myFile.Open(FilePathH.c_str()) ==1; sucess

*** you can then append record here from you sorted structure
void AppendRecord (Record& _addMe);

myFile.Close(); close the sorted file


*************************
for your array  structures

string Name = (schemaLeft.GetAtts().begin()).name;


unsigned int array_size_left = schemaLeft.GetMaxTuples(Name);
Record rec_array[array_size_Left];
//fill rec_array with Records from left table



I hope clarify thing hit me up if you guy have any question

*/


/*
if(join == false){

//this check to see how has the highest amount of comment
if(schemaRight.GetDistincts((schemaRight.GetAtts().back()).name)  >= schemaLeft.GetDistincts((schemaLeft.GetAtts().back()).name)){

while(left->GetNext(temp)){
temp.SetOrderMaker(_omL);
MMrec.insert(pair<Record,int>(temp,0));
}

leftSmaller = true;
}else{

while(right->GetNext(temp)){
temp.SetOrderMaker(_omR);

MMrec.insert(pair<Record,int>(temp,10));
}

leftSmaller = false;
}

join = true;

}



*/




/*1*/

/*	//cout << "TRUEE" << endl;

it = MMrec.begin();
if (it != MMrec.end()) {
Right = (*it);
if (predicate.Run(Left, Right) == 1) {



//Right.print(cout, schemaRight);
cout << endl;
//	Left.print(cout, schemaLeft);
cout << endl;
_myRec.AppendRecords(Left, Right, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts()); //, _kept, numAttKept, startRight);
cout << "APENDED" << endl;
_myRec.print(cout, schemaOut);
cout << endl;

return true;
}
it++;

}*/


/*2*/



/*
if (join == false) {
Record temp;



//cout <<"NOTUPS " <<schemaRight.GetMaxTuples() << endl;
//cout << "NOTUPS "<<schemaLeft.GetMaxTuples() << endl;
//	cout << schemaRight.GetMaxTuples() + schemaLeft.GetMaxTuples() << endl;;


//int sum = schemaRight.GetMaxTuples() + schemaLeft.GetMaxTuples();
// &&   sum != 150025
//if (schemaRight.GetMaxTuples() <= schemaLeft.GetMaxTuples()) {  alex


cout <<" Right " <<schemaRight << endl;
cout <<"Left " <<schemaLeft << endl;


if (schemaRight.GetMaxTuples() <= schemaLeft.GetMaxTuples()) {

cout << "???" << endl;
while (right->GetNext(temp) == true) {

MMrec.push_back(temp);
temp.Nullify();
}
smallerTable = true;

}
else {

while (left->GetNext(temp) == true) {
MMrec.push_back(temp);
temp.Nullify();
}
smallerTable = false;
}
join = true;
}


if (smallerTable == true) {
while (left->GetNext(Left) == true) {



for (it = MMrec.begin(); it != MMrec.end(); it++) {
Right = (*it);

if (predicate.Run(Left, Right) == 1) {

_myRec.AppendRecords(Left, Right, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts()); //, _kept, numAttKept, startRight);
//cout<< " Merge sucess"<<endl;
//	Right.Nullify();
Left.Nullify();


return true;
}
}
}
}
else {

while (right->GetNext(Right) == true) {
//	cout<<"hey"<<endl;


for (it = MMrec.begin(); it != MMrec.end(); ++it) {
//cout<< " Merge try2"<<endl;
Left = (*it);
if (predicate.Run(Left, Right) == 1) {

_myRec.AppendRecords(Right, Left, schemaRight.GetNumAtts(), schemaLeft.GetNumAtts()); //, _kept, numAttKept, startRight);
//cout<< " Merge sucess"<<endl;
Right.Nullify();
//Left.Nullify();


return true;

}
}
}
}*/

//delete _omR,_omL;




/*3*/


/*
while (right->GetNext(temp) == true) {
//	cout << "TRUE" << endl;
MMrec.push_back(temp);
temp.Nullify();
}

//STORAGE IS EMPTY
//First getnext or endoflist and wipe storage and get nextleft
if (storage.GetSize() == 0) {
it = MMrec.begin();
bool emptyCheck = left->GetNext(storage);
if (!emptyCheck)
return false;
}
else {
it++;
}

if (it != MMrec.end()) {
Right = (*it);
if (predicate.Run(storage, Right) == 1) {



//Right.print(cout, schemaRight);
//	cout << endl;
//	Left.print(cout, schemaLeft);
//	cout << endl;
_myRec.AppendRecords(storage, Right, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts()); //, _kept, numAttKept, startRight);
//	_myRec.AppendRecords(Right, Left, schemaRight.GetNumAtts(), schemaLeft.GetNumAtts());
//cout<< " Merge sucess"<<endl;
//	cout << "APENDED" << endl;
//_myRec.print(cout, schemaOut);
//	cout << endl;
//	Right.Nullify();
//	Left.Nullify();
//	Right.Nullify();

return true;
}
}
else {	// we reached end of map, restart from top
storage.Nullify();
it = MMrec.begin();
bool emptyCheck = left->GetNext(storage);

if (!emptyCheck)
return false;


Right = (*it);
if (predicate.Run(storage, Right) == 1) {


_myRec.AppendRecords(storage, Right, schemaLeft.GetNumAtts(), schemaRight.GetNumAtts()); //, _kept, numAttKept, startRight);

return true;



}
*/
