#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <queue>
#include <functional>
#include <sstream>

#include "Schema.h"
#include "Comparison.h"
#include "QueryOptimizer.h"


#include "EfficientMap.cc"
#include "InefficientMap.cc"

using namespace std;


QueryOptimizer::QueryOptimizer(Catalog& _catalog) : catalog(&_catalog) {
	*catalog = _catalog;

}

QueryOptimizer::~QueryOptimizer() {
}

void QueryOptimizer::Optimize(TableList* _tables, AndList* _predicate,
	OptimizationTree* _root) {
	
	//map tablename -> tuple(size,cost,order)
	InefficientMap < KeyString, Keyify<tuple<unsigned int, unsigned int, string> > > Opt; 
	InefficientMap < KeyString, Keyify<tuple<unsigned int, unsigned int, string> > > OptSingle;

	

	TableList* temptab = _tables;
	AndList* tempand = _predicate;
	AndList* tempand2 = _predicate;
	
	vector<string> tables;
	while (temptab != NULL) {
		string name = temptab->tableName;
		tables.push_back(name);
		cout <<"TABS " << name << endl;
		unsigned int size;
		unsigned int init = 0;
		catalog->GetNoTuples(name,size);
		KeyString temp = name;
		tuple<unsigned int, unsigned int, string>  a(size, 0, name);
		Keyify<tuple<unsigned int, unsigned int, string> > b =a;
		Opt.Insert(temp, b);
		
		temptab = temptab->next;
	}
	
	for (Opt.MoveToStart(); !Opt.AtEnd(); Opt.Advance()) {
		tuple<unsigned int, unsigned int, string>  b = Opt.CurrentData();
		cout << "MAP "<<get<2>(b)<<endl;
	}
	//push down '=' 
	
	while (tempand2 != NULL) {

	
		string left = tempand2->left->left->value;
		int op = tempand2->left->code; //operand type compare
		string right = tempand2->left->right->value;  //right to compare

		if (op == 7) // =
		{
			bool FindRight = false;
			for (int i = 0; i < tables.size(); i++)
			{
				unsigned int dist;
				if (catalog->GetNoDistinct(tables[i], right, dist)) {
					//this means the value of right is an attribute and no push down select
					if (dist != 4294967295) {
						//cout << right << endl;
						FindRight = true;
						break;
					}

				}
			}
		
			if (FindRight == false) {
			//	cout << "left " << left << endl;
				for (int i = 0; i < tables.size(); i++)
				{
					//cout << tables[i]<<endl;

					unsigned int dist;
					if (catalog->GetNoDistinct(tables[i], left, dist))
					{ //
					//	cout << tables[i] << endl;
					//	cout << "found " << left << endl;
						//find if right is an attribute
				//		cout << dist << endl;
						if (dist != 4294967295) {
							//found left
							KeyString FindThis = tables[i];
							tuple<unsigned int, unsigned int, string>  a;
							tuple<unsigned int, unsigned int, string>  b(dist, 0, tables[i]);

							a=	Opt.Find(FindThis) ;
							get<0>(a) = get<0>(a) / get<0>(b);

							cout << get<0>(a) << "DIVIDE " << tables[i] << endl;
							//if (get<0>(a) > get<0>(b))
							Opt.Find(FindThis) = a;
						//	get<0>(b) = dist;
							//cout << b << endl;
						}

					}
					//cout << dist << endl;
				}
			}

		}
		


		//cout << left << " " << op << " " << right << endl;   
		//op 7 is '='
		//op 6 is '<'

		tempand2 = tempand2->rightAnd;
	}
	//push down < > 
	//cout << "did i crash yet" << endl;
	while (tempand != NULL) {

		string left = tempand->left->left->value;
		int op = tempand->left->code; //operand type compare
		string right = tempand->left->right->value;  //right to compare
		//no need right since op 5 or 6 is < >

		if (op == 5 || op == 6) {
			
				//cout << "left " << left << endl;
				for (int i = 0; i < tables.size(); i++)
				{
					//cout << tables[i]<<endl;

					unsigned int dist;
					if (catalog->GetNoDistinct(tables[i], left, dist))
					{ //
					  //	cout << tables[i] << endl;
					//  	cout << "found " << left << endl;
					  //find if right is an attribute
					 // 		cout << dist << endl;
						if (dist != 4294967295) {
							//found left
							KeyString FindThis = tables[i];
							tuple<unsigned int, unsigned int, string>  a;

							a = Opt.Find(FindThis);
							//cout << get<0>(a) << endl;

							get<0>(a) = get<0>(a) / 3;
						//	cout << get<0>(a) << endl;
							Opt.Find(FindThis) = a;
							//	get<0>(b) = dist;
							//cout << b << endl;
						}

					}
					//cout << dist << endl;
				}
			}

		
		tempand = tempand->rightAnd;
	}

	
	

	for (Opt.MoveToStart(); !Opt.AtEnd(); Opt.Advance()) {
		tuple<unsigned int, unsigned int, string>  b = Opt.CurrentData();
		//cout << Opt.CurrentKey()<<" " << get<0>(b) << endl;
		Keyify<tuple<unsigned int, unsigned int, string> > c = b;
		KeyString ins = Opt.CurrentKey();
		OptSingle.Insert(ins, c);
	}

	priority_queue<string, vector<string>, greater<string> > q;

	for (int i = 0; i < tables.size(); i++) {
		q.push(tables[i]);

	}

	int count = 0;
	while (!q.empty()) {
		string temp = q.top();
		tables[count] = q.top();
		q.pop();
		count++;
	}


	//map tablename -> tuple(size,cost,order)
	InefficientMap < KeyString, Keyify<tuple<float, float, string> > > OptPair;
	//make map for 2 tables
	if (count > 1) {
		for (int i = 0; i < tables.size(); i++) {
			for (int j = i + 1; j < tables.size(); j++)
			{
				string temper = tables[i] + "," + tables[j];
				KeyString combo = temper;
				float size;
				tuple<unsigned int, unsigned int, string>  a;
				tuple<unsigned int, unsigned int, string>  b;
				KeyString temp = tables[i];
				a = Opt.Find(temp); //find tuple1
				temp = tables[j];
				b = Opt.Find(temp); //find tuple2
			
				size = get<0>(a) * get<0>(b);
			//	cout << get<0>(a) << " " << get<0>(b) << " " << size << endl;

				AndList* tempand3 = _predicate;
				while (tempand3 != NULL) {
					string left = tempand3->left->left->value;
					int op = tempand3->left->code; //operand type compare
					string right = tempand3->left->right->value;  //right to compare
					
					if (op == 7) // =
					{

						unsigned int dist1;
						unsigned int dist2;
						if (catalog->GetNoDistinct(tables[i], left, dist1))
						{
							if (catalog->GetNoDistinct(tables[j], right, dist2))
							{
								if (dist1 != 4294967295 && dist2 != 4294967295) {
									if (dist1 > dist2) {

								
										size = size/ dist1;
									
										break;
									
									}
									else {
									
										size = size/ dist2;
									
										break;
										
									}
								}
							}
						}
						if (catalog->GetNoDistinct(tables[i], right, dist1))
						{
							if (catalog->GetNoDistinct(tables[j], left, dist2))
							{
								if (dist1 != 4294967295 && dist2 != 4294967295) {
									if (dist1 > dist2) {


										size = size / dist1;
									
										break;

									}
									else {

										size = size / dist2;
										
										break;

									}
								}
							}
						}


					}
					tempand3 = tempand3->rightAnd;
				}
				
				string order = "(" + temper + ")";
			//	cout << order << endl;
				tuple<float, float, string>  c(size, 0, order);

				Keyify<tuple<float, float, string> > d = c;
				OptPair.Insert(combo, d);
			}
		}
	}
	else
	{
		
		tuple<unsigned int, unsigned int, string>  one;
		string OneTab;
		for (Opt.MoveToStart(); !Opt.AtEnd(); Opt.Advance()) {
		
			one = Opt.CurrentData();
			OneTab = Opt.CurrentKey();
		}
		
		_root->tables.push_back(OneTab);
		_root->tuples.push_back(get<0>(one));
		_root->noTuples = (get<0>(one));
		//_root->tables = tabs;

	}
	

	if (count > 1) {
		float min = 999999999999999999;
		string minS = "";
		for (OptPair.MoveToStart(); !OptPair.AtEnd(); OptPair.Advance()) {
			tuple<float, float, string>  b = OptPair.CurrentData();
			if (get<0>(b) < min)
			{
				min = get<0>(b);
				minS = OptPair.CurrentKey();
			}
		}
		//cout << minS << endl;

		KeyString find = minS;
		tuple<float, float, string>  b = OptPair.Find(find);
		Keyify<tuple<float, float, string>> c = b;

		InefficientMap<KeyString, Keyify<tuple<float, float, string> > > EndMap;

		EndMap.Insert(find, c);

		istringstream ss(OptPair.CurrentKey());
		string token;
		Keyify<tuple<unsigned int, unsigned int, string>> temp;
		while (getline(ss, token, ',')) {
			//cout << token << '\n';
			KeyString remove = token;
			Opt.Remove(remove, remove, temp);
		}
		//cout << "tables left" << endl;

		/*
		for (Opt.MoveToStart(); !Opt.AtEnd(); Opt.Advance()) {
			cout << Opt.CurrentKey() << endl;

		}*/


		//greed
		//minS is the list smallest string
		//cout << "minS " << minS << endl;
		int tableL = minS.length();


		//	cout << "Length"<<Opt.Length() << endl;
		//GREEDY STARTS HERE
		while (Opt.Length() != 0) {
			for (Opt.MoveToStart(); !Opt.AtEnd(); Opt.Advance()) {

				string add = minS;
				add = add + ',';
				string tempS = Opt.CurrentKey();  //table 2
				add = add + tempS;
			//	cout << "added" << add << endl;
				KeyString key = minS;

				tuple<float, float, string> a = EndMap.Find(key);;
				tuple<unsigned int, unsigned int, string>  b = Opt.Find(Opt.CurrentKey());

				float size = get<0>(a) * get<0>(b);
			//	cout << get<0>(a) << " " << get<0>(b) << " " << size << endl;
				//cout << minS << endl;
				istringstream sss(minS);
				string tokens; //table1 table2 = tempS
				while (getline(sss, tokens, ',')) {
					AndList* tempand3 = _predicate;
					while (tempand3 != NULL) {
						string left = tempand3->left->left->value;
						int op = tempand3->left->code; //operand type compare
						string right = tempand3->left->right->value;  //right to compare
			//			cout << tokens << " " << tempS << endl;
						if (op == 7) // =
						{

							unsigned int dist1;
							unsigned int dist2;
							if (catalog->GetNoDistinct(tokens, left, dist1))
							{
								if (catalog->GetNoDistinct(tempS, right, dist2))
								{
									if (dist1 != 4294967295 && dist2 != 4294967295) {
										if (dist1 > dist2) {


											size = size / dist1;

											break;

										}
										else {

											size = size / dist2;

											break;

										}
									}
								}
							}
							if (catalog->GetNoDistinct(tokens, right, dist1))
							{
								if (catalog->GetNoDistinct(tempS, left, dist2))
								{
									if (dist1 != 4294967295 && dist2 != 4294967295) {
										if (dist1 > dist2) {


											size = size / dist1;

											break;

										}
										else {

											size = size / dist2;

											break;

										}
									}
								}
							}


						}
						tempand3 = tempand3->rightAnd;
					}
				}

				//	cout << size << endl;
				string order = "(" + get<2>(a) + tempS + ")";
				//cout << order << endl;
				tuple<float, float, string>  c(size, get<0>(a) + size, order);
				Keyify<tuple<float, float, string>> d = c;

				KeyString adding = add;
				//cout << adding << endl;
				EndMap.Insert(adding, d);
			//	cout << EndMap.Length() << endl;
				//	cout << Opt.Length() << endl;
					//break;
			}
			string endKey;
			float minCheck = 999999999999999999;
			for (EndMap.MoveToStart(); !EndMap.AtEnd(); EndMap.Advance()) {
				endKey = EndMap.CurrentKey();
				tuple<float, float, string>  b = EndMap.CurrentData();

				if (endKey.length() > tableL)
				{

					if (get<0>(b) <= minCheck)
					{
						minCheck = get<0>(b);
						minS = endKey;
					//	cout << "here is min" << endl;
					}
				}

			}

			string remove = minS;
		//	cout << "this is minS ";
	//		cout << minS << endl;
			remove.erase(0, tableL + 1);
			KeyString removed = remove;
			tableL = minS.length();
//			cout << remove << endl;
			Keyify<tuple<unsigned int, unsigned int, string>> temptup;
			Opt.Remove(removed, removed, temptup);


		}




		cout << "donezo" << endl;
		/*
		for (OptPair.MoveToStart(); !OptPair.AtEnd(); OptPair.Advance()) {
			tuple<float, float, string>  b = OptPair.CurrentData();
			//cout << "ehh";

			istringstream ss(OptPair.CurrentKey());
			string token;
			while (getline(ss, token, ',')) {
				cout << token << '\n';
			}
			//cout << endl;
			cout<<OptPair.CurrentKey()<< " "  << get<0>(b) <<" "  <<get<2>(b) << endl;
		}*/


	//	cout << minS << endl;
		KeyString endFind = minS;
		tuple<float, float, string>  endGame = EndMap.Find(endFind);
		//cout << get<0>(endGame) << " " << get<1>(endGame) << " " << get<2>(endGame) << endl;

		vector<string> SchTable;

		istringstream sst(minS);
		string test;
		int counter=0;
		while (getline(sst, test, ',')) {
			SchTable.push_back(test);
			counter++;
			//cout << test << endl;
		}
		//build top root
		/*
		for (int i = 0; i < SchTable.size(); i++) {
			_root->tables.push_back(SchTable[i]);
			KeyString finded = SchTable[i];
			tuple<unsigned int, unsigned int, string>  aa = OptSingle.Find(finded);
			_root->tuples.push_back(get<0>(aa));
		}*/
		//_root->noTuples = get<1>(endGame); //float
		//cout <<"#TABLES" <<counter << endl;
		//_root->leftChild =  new OptimizationTree();
		//_root->rightChild = new OptimizationTree();
	
		


		string prev = "A";
		string current = "";
		for (EndMap.AtStart(); !EndMap.AtEnd(); EndMap.Advance()) {

			if (prev != "A") {
				current = EndMap.CurrentKey();
				//cout << "current :" << current << endl;
				//cout << "prev :" << prev << endl;
				_root->leftChild = new OptimizationTree();
				_root->rightChild = new OptimizationTree();
				_root->rightChild->parent = new OptimizationTree();
				_root->leftChild->parent = new OptimizationTree();
				int lent = current.length();
				prev.erase(0, lent + 1);
				//cout << "new Prev:" << prev << endl;
				_root->rightChild->tables.push_back(prev);
				KeyString rightside = prev;
				tuple<unsigned int, unsigned int, string>  aaaaa = OptSingle.Find(rightside);
				_root->rightChild->noTuples = get<0>(aaaaa);
				_root->rightChild->tuples.push_back(get<0>(aaaaa));
				_root->rightChild->parent = _root;
				_root->leftChild->parent = _root;
				OptimizationTree* temp = new OptimizationTree();
			//	_root->parent = new OptimizationTree();
			//	temp = _root;
			//	_root->parent = _root;
				_root = _root->leftChild;
				//cout << "RIGHT " << endl;
				//cout <<"RIGHT " <<_root->rightChild->tables[0] << endl;
			}

			//cout << EndMap.CurrentKey() << endl;
			istringstream sst(EndMap.CurrentKey());
			string test;
			while (getline(sst, test, ',')) {
			//	if(_root->tables.size() < counter)
					_root->tables.push_back(test);
				KeyString Findf = test;
				tuple<unsigned int, unsigned int, string>  aa = OptSingle.Find(Findf);
			//	if (_root->tuples.size() < counter)
				_root->tuples.push_back(get<0>(aa));
				//cout << test << endl;
			}
			tuple<float, float, string>  endGames = EndMap.Find(EndMap.CurrentKey());
			if(get<1>(endGames) != 0)
				_root->noTuples = get<1>(endGames);
			else
				_root->noTuples = get<0>(endGames);
			//cout << "size"<<get<0>(endGames) << endl;

			for (int j = 0; j < _root->tables.size(); j++)
			{
				cout << _root->tables[j] << endl;
			}
			cout << endl;
			prev = EndMap.CurrentKey();
		}

		_root->leftChild = new OptimizationTree();
		_root->rightChild = new OptimizationTree();
		_root->rightChild->parent = new OptimizationTree();
		_root->leftChild->parent = new OptimizationTree();
		string left = _root->tables[0];
		string right = _root->tables[1];
	
		_root->rightChild->tables.push_back(right);
		//cout << _root->rightChild->tables[0] << endl;
		KeyString rightside = right;
		tuple<unsigned int, unsigned int, string>  aaaaa = OptSingle.Find(rightside);
		_root->rightChild->noTuples = get<0>(aaaaa);
		_root->rightChild->tuples.push_back(get<0>(aaaaa));
		_root->rightChild->parent = _root;
		_root->leftChild->parent = _root;
		KeyString leftside =left;
		_root = _root->leftChild;
		aaaaa = OptSingle.Find(leftside);
		_root->noTuples = get<0>(aaaaa);
		_root->tuples.push_back(get<0>(aaaaa));
		_root->tables.push_back(left);
	
		/*
		cout << "testing" << endl;

		
		while (_root->parent != NULL) {
			for (int i = 0; i < _root->tables.size(); i++) {
				cout << _root->tables[i] << " ";
			}
			if (_root->parent->rightChild != NULL)
			{
				cout << "\t" << "\t" << "\t" << _root->parent->rightChild->tables[0] << endl;
			}
			_root = _root->parent;

		}*/



		/*
		_root->leftChild = new OptimizationTree();
		_root->rightChild = new OptimizationTree();
		_root->rightChild->parent = new OptimizationTree();
		_root->leftChild->parent = new OptimizationTree();
	
		_root->rightChild->tables.push_back(_root->tables[1]);
		KeyString rightside = _root->tables[1];
		tuple<unsigned int, unsigned int, string>  aaaaa = OptSingle.Find(rightside);
		_root->rightChild->noTuples = get<0>(aaaaa);
		_root->rightChild->tuples.push_back(get<0>(aaaaa));
		_root->rightChild->parent = _root;
		_root->leftChild->parent = _root;
		_root = _root->leftChild;
		_root->tables.push_back(_root->tables[0]);
		KeyString leftside = _root->tables[0];
		aaaaa = OptSingle.Find(leftside);
		_root->noTuples = get<0>(aaaaa);
		_root->tuples.push_back(get<0>(aaaaa));
		*/
		/*
		cout << _root->tables.size() << endl;
		cout << _root->parent->tables[0] << endl;
		cout << _root->parent->tables[1] << endl;
		cout << _root->parent->tables[2] << endl;
		*/
	
	//	cout << _root->tables[0] << endl;
		/*
		cout << _root->tables[0] << endl;
		cout << _root->tables[1] << endl;
		//cout << _root->tables[2] << endl;
		//cout << _root->tables.size() << endl;
		//cout << _root->parent->tables.size() << endl;
		cout << endl;
		cout << _root->parent->tables[0] << endl;
		cout << _root->parent->tables[1] << endl;
		cout << _root->parent->tables[2] << endl;
		cout << _root->parent->tables[3] << endl;
		cout << _root->parent->tables[4] << endl;
		cout << _root->parent->tables.size() << endl;
		*/
		//cout << _root->parent->tables[3] << endl;
	//	cout << _root->parent->parent->tables.size();
	//	cout << _root->parent->rightChild->tables.size() << endl;
	//	cout << _root->parent->parent->tables.size() << endl;
		//ALL YOU NEED LEFT IS LEFT CHILD AND RIGHT CHILD THEN YOU GUCCI!!!!

//		cout << _root->parent->tables[0] << endl;
	//	cout << _root->tables[2] << endl;
		//cout << counter << endl;
		//asuming counter > 2;
		
		/*
		for (OptSingle.AtStart(); !OptSingle.AtEnd(); OptSingle.Advance()) {
			cout << OptSingle.CurrentKey() << endl;
		}*/

		//cout << SchTable[0];
		//string inse = SchTable[0];
		
		//_root->tables.push_back(SchTable[0]);
		//KeyString findKey = SchTable[0];
		//tuple<unsigned int, unsigned int, string>  a = OptSingle.Find(findKey);
		//_root->tuples.push_back();
		//_root->parent = new OptimizationTree();
		//_root->parent->rightChild = new OptimizationTree();
		//_root->parent->rightChild->tables.push_back(SchTable[1]);


		/*	
		for (EndMap.AtStart(); !EndMap.AtEnd(); EndMap.Advance()) {
			//cout << EndMap.CurrentKey() << endl;
		//	_root->tables.push_back(EndMap.CurrentKey());
			tuple<float, float, string>  PuttPutt = EndMap.CurrentData();
			string push = EndMap.CurrentKey();
			istringstream sstt(push);
			while (getline(sstt, push, ',')) {
				_root->tables.push_back(push);
				//cout << test << endl;
			}

			_root->noTuples = get<1>(PuttPutt);

		//_root->noTuples
		}*/


		//_root = _root->parent;
		//_root = _root->rightChild;
		//_root->tables.push_back(SchTable[1]);
		//_root->leftChild->tables.push_back(SchTable[0]);


	}


	// compute the optimal join order

}
