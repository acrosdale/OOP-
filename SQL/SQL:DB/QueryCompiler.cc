#include "QueryCompiler.h"
#include "QueryOptimizer.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include "RelOp.h"
#include <queue>
#include <functional>
#include <sstream>
#include <iostream> 

#include "EfficientMap.cc"
#include "InefficientMap.cc"

using namespace std;


QueryCompiler::QueryCompiler(Catalog& _catalog, QueryOptimizer& _optimizer) :
	catalog(&_catalog), optimizer(&_optimizer) {
	*catalog = _catalog;
	*optimizer = _optimizer;
}


EfficientMap<KeyString, Keyify<Scan*> >  Scanned;
EfficientMap<KeyString, Keyify<Select*>>  Selected;

EfficientMap<KeyString, Keyify<ScanIndex*> > Indexed;

QueryCompiler::~QueryCompiler() {
}

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {

	
	cout << "JUST CALLED THIS" << endl;
	vector<string> AttToSrch;
	vector<string> TypeToSrch;
	vector<unsigned int> DistToSrch;

	AttToSrch.push_back("Number");//int
	AttToSrch.push_back("Keys");//string
	AttToSrch.push_back("PageNumbers");			//string
	AttToSrch.push_back("RecordIndex");  //record
	AttToSrch.push_back("Child");	//p_child->pagenum
	AttToSrch.push_back("Leaf");			//int leaf

	TypeToSrch.push_back("INTEGER");
	TypeToSrch.push_back("STRING");
	TypeToSrch.push_back("STRING");
	TypeToSrch.push_back("STRING");
	TypeToSrch.push_back("STRING");
	TypeToSrch.push_back("INTEGER");


	DistToSrch.push_back(1);
	DistToSrch.push_back(1);
	DistToSrch.push_back(1);
	DistToSrch.push_back(1);
	DistToSrch.push_back(1);
	DistToSrch.push_back(1);

	Schema toSrch(AttToSrch,TypeToSrch,DistToSrch);
	cout << "WHAT HAPPENED" << endl;

	string s = "lineitem";
	Schema findS;
	string attFind = "l_discount";
	findS.GetDistincts(attFind);

	cout << "START SCANS" << endl;

	// create a SCAN operator for each table in the query
	vector<string> TableNames;

	TableList * tabs = _tables;
	while (tabs != NULL) {
		Schema s;
		string Name = tabs->tableName;
		//TableNames.push_back(Name);
		string Name2 = Name;
		catalog->GetSchema(Name, s);

		//	cout << s << endl;

		DBFile DBF;

		string filename;
		catalog->GetDataFile(Name, filename);
		//DBF.Create(filename,);
		int i = DBF.Open(filename.c_str());
		Record rec;

		CNF compare;
		
		bool FoundIndex = false;
		//	cout << s << endl;
		compare.ExtractCNF(*_predicate, s, rec);
		if (compare.numAnds > 0) {
			AndList* tempand2 = _predicate;
		
			FoundIndex = false;
			while (tempand2 != NULL) {
				string left = tempand2->left->left->value;
				int op = tempand2->left->code; //operand type compare
				string right = tempand2->left->right->value;  //right to compare
				cout << "DOES THIS CRASH" << endl;
				string indexname = catalog->FindIndex(Name, left);			
				cout << "IT DID" << endl;
				/*
				#define LESS_THAN 5
				#define GREATER_THAN 6
				#define EQUALS 7
				*/

				/*
				ScanIndex(Schema& _schema, Schema& _toSendSch, DBFile& _index, DBFile& _header,
				CNF& _predicate, Record& _constants, string& _table, string& _attribute );

				*/

				if (indexname != ""  && FoundIndex == false) { // found an index on this att
					if (op == 5 || op == 6) {   // < OR >
						string indexfilename;
						DBFile DBF2;
						DBF2.Open(indexfilename.c_str());
						catalog->GetDataFile(indexname, indexfilename);
						//ScanIndex(schema, cnf, rec, DBF, indexfilename   )
						ScanIndex *temp = new  ScanIndex(s, toSrch, DBF2, DBF, compare, rec, Name, left);
						Keyify<ScanIndex*> keyscanindex = temp;
						KeyString key2 = Name2;
						Indexed.Insert(key2, keyscanindex);
						FoundIndex = true;
						break;
					}
					if (op == 7) {
						unsigned int dist;
						if (catalog->GetNoDistinct(indexname, right, dist)  == false)
						{

							string indexfilename;
							DBFile DBF2;
							DBF2.Open(indexfilename.c_str());
							catalog->GetDataFile(indexname, indexfilename);
							//ScanIndex(schema, cnf, rec, DBF, indexfilename   )
							ScanIndex *temp = new  ScanIndex(s, toSrch, DBF2, DBF, compare, rec, Name, left);
							Keyify<ScanIndex*> keyscanindex = temp;
							KeyString key2 = Name2;
							Indexed.Insert(key2, keyscanindex);
							FoundIndex = true;
							break;
						}

					}



				}





				tempand2 = tempand2->rightAnd;
			}



			//if(catalog->FindIndex(Name,))
			//else
			if (FoundIndex == false) {
				Scan * start = new Scan(s, DBF);
				KeyString Key = Name;
				Keyify<Scan*>  scanned = start;
				Scanned.Insert(Key, scanned);

				Select * sel = new Select(s, compare, rec, start);
				//	cout << "SELECT " << s << endl;
				Keyify<Select*> insSel = sel;

				KeyString Key2 = Name2;
				Selected.Insert(Key2, insSel);
			}
			//	cout << s << endl;
		}
		else
		{
			Scan * start = new Scan(s, DBF);
			KeyString Key = Name;
			Keyify<Scan*>  scanned = start;
			Scanned.Insert(Key, scanned);
		}


		

		tabs = tabs->next;
		//DBF.Close();
	}

		cout << "CHECK MAP" << endl;

		//cout << Scanned << endl;
		// push-down selections: create a SELECT operator wherever necessary

		/*
		Record reccc;

		CNF compared;
		Schema test;
		string tableO = "orders";
		catalog->GetSchema(tableO, test);
		compared.ExtractCNF(*_predicate, test, reccc);
		cout <<"orders " <<compared.numAnds << endl;
		*/



		// call the optimizer to compute the join order
	OptimizationTree* root = new OptimizationTree();
	optimizer->Optimize(_tables, _predicate, root);


	//cout << "testing here" << endl;
	//cout << root->tables[0] << endl;

	//for (int i = 0; i < root->tables.size(); i++) {
		//	cout << root->tables[i] << " ";
	//}
	//	cout << root->noTuples;
		//cout << endl;
		//cout << root->parent->tables[0] << endl;
	RelationalOp *end;

	for (int i = 0; i < root->tables.size(); i++) {
		TableNames.push_back(root->tables[i]);
		//cout << root->tables[i] << " ";

	}


	if (root->tables.size() > 1) {
		while (root->leftChild != NULL) {

			for (int i = 0; i < root->tables.size(); i++) {
				cout << root->tables[i] << " ";
			
			}
			cout << root->noTuples;
			if (root->rightChild != NULL)
			{
				cout << "\t" << "\t" << "\t" << root->rightChild->tables[0] << " ";
				cout << root->rightChild->noTuples << endl;
			}
			if (root->leftChild->leftChild == NULL)
				break;

			root = root->leftChild;
		}


		//cout << root->tables[0] << " " << root->noTuples << endl;

		//cout << root->tables[1] << " " << root->noTuples << endl;
		//Schema test1;
		//catalog->GetSchema(root->tables[0]);
		//string a = "partsupp";
		//catalog->GetSchema(a, test1);
		//cout << test1 << endl;
		//Schema test2;

		/*
		for (int i = 0; i < root->tables.size(); i++) {
			catalog->GetSchema(root->tables[i], s);
		}
		cout << "schema" << s << endl;*/


		Schema s;
		root = root->leftChild;
		cout << root->tables[0] << " " << root->noTuples << endl;

		/*
		Schema Left;
		string leftTab = "part";
		Schema Right;
		string rightTab = "partsupp";
		catalog->GetSchema(leftTab, Left);
		catalog->GetSchema(rightTab, Right);

		AndList  temp = *_predicate;
		CNF Comp;

		Comp.ExtractCNF(temp, Left, Right);
		cout << "COMPARED VALUES" << Comp.numAnds << endl;
		*/

		Schema combined;
		end = Build(root, end, _predicate); // build first left
		//cout << *end << endl;
		root = root->parent;
		end = Build(root, end, _predicate);

		//_queryTree.SetRoot(*end);


		//cout <<"TOP " <<*end << endl;
		//Project
	}
	else
	{


		end = Build(root, end, _predicate);

		///_queryTree.SetRoot(*end);


	//	cout << *end << endl;

	}

	//	string s(tempF->leftOperand->value); prints out char*s
//	cout << "function" << endl;
	//only for add

	Schema check;
	/*
	
	for (int i = 0; root->tables.size(); i++) {
		string name = root->tables[i];
		Schema temp;
		catalog->GetSchema(name, temp);
		check.Append(temp);
	}*/

	Schema All;
	for (int i = 0; i < TableNames.size(); i++) {
		string name = TableNames[i];
		Schema temp;
		catalog->GetSchema(name, temp);
		All.Append(temp);
	}

	cout << " ALL "<< All << endl;
	cout << " CHECK " << endl;

	Schema remove; //distinct
				   //project

	Schema outPro;//project
	cout << "DID WE CRASH" << endl;
	//project -> distinct
	if (_distinctAtts) {
		NameList* selectatts = _attsToSelect;
		cout << "attributes" << endl;
		int numIn = 0;
		Schema inPro;
		//Schema outPro;
		int numOut = 0;
		while (selectatts != NULL)
		{
			//cout << selectatts->name << endl;
			string s = selectatts->name;
			Schema sche;

			TableList* temp = _tables;
			while (temp != NULL) {
				string name = temp->tableName;
				//	cout << "BANE " << name << endl;
				unsigned int test;
				if (catalog->GetNoDistinct(name, s, test)) {
					if (test != 4294967295) {
						catalog->GetSchema(name, sche);
						int index;
						inPro.Append(sche);
						index = sche.Index(s);
						vector<int>keep;
						keep.push_back(index);
						sche.Project(keep);
						outPro.Append(sche);
					}

				}
				temp = temp->next;

			}
			selectatts = selectatts->next;
			numOut++;
		}



		remove = outPro;//this is in distinct

		numIn = inPro.GetAtts().size();

		vector<Attribute> output = outPro.GetAtts();
		int * keep = new int[numOut];
		//	cout << numOut << " " << output.size() <<endl;

		for (int i = 0; i < output.size(); i++) {
			string n = output[i].name;
			//cout << inPro.Index(n) << endl;
			keep[i] = inPro.Index(n);

		}
		//cout << "INPRO ";
		//cout << inPro << endl;
		cout << All << endl;
		cout << outPro << endl;
		Project* pro = new Project(All, outPro, numIn, numOut, keep, end);
		end = pro;
		cout << outPro << endl;
		DuplicateRemoval * dupe = new DuplicateRemoval(outPro, end);
		end = dupe;
		//	cout << *end;
	}
	//SUM OR GROUP BY SUM
	else if (_finalFunction != NULL) {
		cout << "DID WE CRASH" << endl;
		cout << "sum" << endl;
		//	cout << _finalFunction->leftOperand->value << endl;
		//FuncOperator
		Function * parse = new Function();
		FuncOperator * tempF = _finalFunction;
		Schema in; // find func
		Schema out;
		/*
		while (tempF != NULL) {
		//cout << tempF->code << endl;
		if ((tempF->leftOperator == NULL) && (tempF->right == NULL)) {
		if (tempF->leftOperand->code == NAME) {

		string s(tempF->leftOperand->value);
		//cout << tempF << endl;
		cout << "TABLE "<<s << endl;
		int p(tempF->leftOperand->code);
		cout << p << endl;
		Schema sche;

		for (int i = 0; i < TableNames.size(); i++) {
		string name = TableNames[i];
		cout << name << endl;
		unsigned int test;
		if (catalog->GetNoDistinct(name, s, test)) {
		if (test != 4294967295) {
		catalog->GetSchema(name, sche);
		int index;
		index = sche.Index(s);
		vector<int>keep;
		keep.push_back(index);
		in.Append(sche);
		sche.Project(keep);
		out.Append(sche);

		}

		}

		}
		}

		}

		tempF = tempF->right;
		}
		*/



		vector<string> a;
		Schema temp;

		//THIS SHIT MAKES SCHEMA
		FINDTHISSHIT(tempF, temp, a);
		//parse->GrowFromParseTree(_finalFunction, out);

		for (int i = 0; i < TableNames.size(); i++) {
			string name = TableNames[i];
			cout << name << endl;
			Schema sche;


			unsigned int test;
			for (int i = 0; i < a.size(); i++) {
				if (catalog->GetNoDistinct(name, a[i], test)) {
					if (test != 4294967295) {
						//	cout << "HERE" << endl;
						catalog->GetSchema(name, sche);
						int index;
						index = sche.Index(a[i]);
						vector<int>keep;
						keep.push_back(index);
						in.Append(sche);
						sche.Project(keep);
						out.Append(sche);

					}

				}
			}
		}
		FuncOperator * tempF2 = _finalFunction;
		parse->GrowFromParseTree(tempF2, All);
		cout << "THIS IS TEMP" << endl;
		cout << temp << endl;
		cout << out << endl;
		cout << All << endl;
		//cout << "in sum" << in << endl;
		//in in the schema taking in tables of what is sum
		//out should be what is being summed


		vector<string> Sname;
		Sname.push_back("sum");
		vector<string> Type;
		bool isInt = true;

		vector<Attribute> att = out.GetAtts();
		for (int i = 0; i < att.size(); i++) {
			if (att[i].type == Float) {
				isInt = false;
			}
		}

		if (isInt == true)
		{
			Type.push_back("INTEGER");
		}
		else
			Type.push_back("FLOAT");

		vector<unsigned int> intts;
		intts.push_back(1);

		Schema sum(Sname, Type, intts);
		cout << "OUT OUT OUT " << out << endl;


		if (_groupingAtts != NULL) {
			NameList* groupatts = _groupingAtts;
			//cout << "grouping" << endl;

			Schema inGroup;
			//Schema out;
			int attno = 0;
			while (groupatts != NULL)
			{
				string s = groupatts->name;
				cout << "GROUP ATT " << groupatts->name << endl;
				Schema sche;


				TableList* temp = _tables;
				while (temp != NULL) {
					string name = temp->tableName;
					//cout << "BANE " << name << endl;
					unsigned int test;
					if (catalog->GetNoDistinct(name, s, test)) {
						if (test != 4294967295) {
							catalog->GetSchema(name, sche);
							int index;
							index = sche.Index(s);
							vector<int>keep;
							keep.push_back(index);
							sche.Project(keep);
							inGroup.Append(sche);
							attno++;
							cout << sche << endl;
						}

					}
					temp = temp->next;

				}

				groupatts = groupatts->next;
			}

			vector<Attribute> a;
			a = inGroup.GetAtts();
			cout << inGroup << endl;
			int*atts = new int[attno];
			priority_queue<string, vector<string>, greater<string> > q;
			for (int i = 0; i < a.size(); i++) {
				q.push(a[i].name);
			}
			int countAtt = 0;
			int orderAtt;


			while (!q.empty()) {
				string temp = q.top();
				cout << "ATT" << temp << endl;
				for (int i = 0; i < a.size(); i++) {
					if (temp == a[i].name) {
						//orderAtt = i;
						atts[countAtt] = i;
						countAtt++;
						break;
					}
				}

				q.pop();
			}


			//Function * func;
			OrderMaker * odd = new OrderMaker(inGroup, atts, attno);
			out.Append(inGroup);
			Schema tempsum = sum;
			Schema tempall = All;
			sum.Append(inGroup);
			tempall.Append(tempsum);
			/*
			cout << "OUT" << endl;
			cout << tempall << endl;
			cout << "sum" << endl;
			cout << sum << endl;
			cout << "ALL" << endl;
			cout << All << endl;
			cout << "INGROUP" << endl;
			cout << inGroup << endl;
			*/
			//		cout << "sum" << endl;
			//		cout << sum << endl;
			//		cout << "ALL" << endl;
		//			cout << All << endl;
			int AllSize = All.GetNumAtts();
			//	GroupBy * group = new GroupBy(All, sum, *odd, *parse, end);
			//	end = group;


			if (_attsToSelect != NULL) {
				//sum is sum,groupingAtt
				//tempall is All+sum+groupingAtt
				//select Sum(att), GroupingAtt




				GroupBy * group = new GroupBy(All, sum, *odd, *parse, end);
				//	GroupBy * group = new GroupBy(tempall, sum, *odd, *parse, end);
				end = group;
				outPro = sum;
				//outPro = tempsum;
				//cout << "OUTPROOO" << tempsum << endl;
				/* any group by will be followed with select sum, gropingatt
			}
			//only need project sum
			//groupy by ->project sum->
			else {
				//All.Append(tempsum);
				//tempall = all schema +sum+groupingAtt
				//tempsum is just sum
				outPro = tempsum;
				int numIn = tempall.GetNumAtts();
				int numOut = outPro.GetNumAtts();
				//keep sum
				int keep[] = {numIn-1};
				Project* pro = new Project(tempall, outPro, numIn, numOut, keep, end);
				end = pro;
				*/


			}

			//	cout << *end;
		}
		else {
			cout << sum << endl;
			cout << All << endl;
			//All.Append(sum);
			//Sum *sumthis = new Sum(All, sum, *parse, end);
			Sum *sumthis = new Sum(All, sum, *parse, end);
			//Sum * sumthis;
			end = sumthis;
			outPro = sum;
		}
		//	cout << *end;
	}
	//nothing but project
	else {
		NameList* selectatts = _attsToSelect;
		cout << "attributes" << endl;
		int numIn = 0;
		Schema inPro;
		//Schema outPro;
		int numOut = 0;


		while (selectatts != NULL)
		{
			cout << selectatts->name << endl;
			string s = selectatts->name;
			Schema sche;

			TableList* temp = _tables;
			while (temp != NULL) {
				string name = temp->tableName;
				//	cout << "BANE " << name << endl;
				unsigned int test;
				if (catalog->GetNoDistinct(name, s, test)) {
					if (test != 4294967295) {
						catalog->GetSchema(name, sche);
						int index;
						inPro.Append(sche);
						index = sche.Index(s);
						vector<int>keep;
						keep.push_back(index);
						sche.Project(keep);
						outPro.Append(sche);
					}

				}
				temp = temp->next;

			}
			selectatts = selectatts->next;
			numOut++;
		}



		remove = outPro;//this is in distinct

		numIn = inPro.GetAtts().size();

		vector<Attribute> output = outPro.GetAtts();
		int * keep = new int[numOut];
		//	cout << numOut << " " << output.size() <<endl;

		for (int i = 0; i < output.size(); i++) {
			string n = output[i].name;
			//cout << inPro.Index(n) << endl;
			keep[i] = inPro.Index(n);

		}
		cout << "INPRO ";
		//cout << inPro << endl;
		cout << outPro << endl;
		cout << numOut << endl;
		cout << outPro.GetNumAtts() << endl;
		Project* pro = new Project(All, outPro, numIn, numOut, keep, end);
		end = pro;
		//	cout << *end;

	}







	//Schema endSchema;
	//write out
	/*
	project ->distinct -> writeout
	sum ->writeout
	groupby ->writeout

	*/
	string file;
	cout << "OUT " << outPro << endl;
	NameList* groupatts = _groupingAtts;

	if (groupatts == NULL) {
		WriteOut * write = new WriteOut(outPro, file, end);

		end = write;
		//cout << "NULL" << endl;
	}


		

	
	_queryTree.SetRoot(*end);

	//cout << _finalFunction->leftOperand<< endl;
	//cout << _finalFunction->leftOperand->code << endl;
//	cout << _finalFunction->leftOperand->value << endl;
	//cout<< _finalFunction->right->
	//first join
	/*
	string Name = root->tables[0];
	KeyString find = Name;
	cout << "about to crash" << endl;
	Scan * canned = Scanned.Find(find);
	Record rec;
	Schema left;
	cout << Name << endl;
	cout << "Scanned "<<*canned << endl;
	catalog->GetSchema(Name, left);
	cout << "about to crash2" << endl;
	Select * sel2;
	CNF compare;
	Join* join;*/


	/*
	compare.ExtractCNF(*_predicate, left, rec);
	RelationalOp* leftRe;
	RelationalOp* rightRe;
	if (compare.numAnds > 0) {
		cout << "about to crash3" << endl;
		sel2 = Selected.Find(find);
		leftRe = sel2;

	}
	else
	{
		leftRe = canned;

	}

	_queryTree.SetRoot(*leftRe);

	//Schema right;
	Schema end; // out schema
	CNF compareAtt;

	while (root->parent != NULL) {
		if (root->rightChild != NULL)
		{
			Schema right;
			Name = root->rightChild->tables[0];
			find = Name;
			Scan *canned2 = Scanned.Find(find);

			catalog->GetSchema(Name, right);

			//compare.ExtractCNF(*_predicate, right, rec);

			if (compare.numAnds > 0) {
				Select * sel3;

				KeyString find2 = find;
				cout << find2 << endl;
				sel3 = Selected.Find(find2); //crashes here
				rightRe = sel3;

				compareAtt.ExtractCNF(*_predicate, left, right);
				join = new Join(left, right, end, compareAtt,leftRe, rightRe);
			}
			else
			{cout << "RIGHT PREE JOIN" << endl << right << endl;
				rightRe = canned2;
				compareAtt.ExtractCNF(*_predicate, left, right);
				join = new Join(left, right, end, compareAtt, leftRe, rightRe);
			}
				cout << " LEFT" << endl;
				cout << left << endl<<endl;
				cout << "RIGHT " << endl << right << endl;
				cout << end << endl;
			left = end;
			leftRe = join;
		}

		root = root->parent;
	}
	cout << " LEFT" << endl;
	cout << left << endl;
	cout<<"RIGHT " << endl << right << endl;
	cout << "did joins" << endl;
//	cout << _finalFunction->code << endl;
	cout << end << endl;
	Schema Pro;


	int num;
	int num2;
	int * k;
	Project* proj = new Project(end, Pro, num, num2, k, join);
	string s;
	WriteOut * w = new WriteOut(end, s, proj);
	cout << s << endl;
	_queryTree.SetRoot(*w);



	//QueryExecutionTree* start = new QueryExecutionTree();
	/*
	int start = 0;
	//initial root left
	KeyString find = root->tables[0];
	Scan * canned = Scanned.Find(find);
	Record rec;
	Schema s;
	string Name = root->tables[0];
	catalog->GetSchema(Name, s);
	Select * sel2;
	CNF compare;
	Join* join;



	compare.ExtractCNF(*_predicate, s, rec);
	if (compare.numAnds > 0) {
		cout << "ehh";
		sel2 = Selected.Find(find);
		_queryTree.SetRoot(*sel2);
	}
	else
	{
		_queryTree.SetRoot(*canned);
	}






	/*
	while (root->parent != NULL) {



	}
	// create join operators based on the optimal order computed by the optimizer

	// create the remaining operators based on the query


	cout <<_finalFunction->code << endl;
	cout << _finalFunction->leftOperand->code << endl;
	cout << _finalFunction->leftOperator << endl;
	cout << _finalFunction->right << endl;
	//connect everything in the query execution tree and return
	*/
	// free the memory occupied by the parse tree since it is not necessary anymore

}


RelationalOp * QueryCompiler::Build(OptimizationTree* rot, RelationalOp * rel, AndList* _predicate) {

	RelationalOp * retu;

	if (rot->leftChild == NULL  && rot->rightChild == NULL) //at a leaf
	{
		KeyString findleaf = rot->tables[0];

		cout << "TUPLES AT " << rot->tables[0] << " " << rot->tuples[0] << endl;
		if (Selected.IsThere(findleaf)) {

			retu = Selected.Find(findleaf);


			//Select * sel = retu;

			rel = retu;
			return retu;
		}
		else if (Indexed.IsThere(findleaf)) {

			retu = Indexed.Find(findleaf);
			rel = retu;
			return retu;
		}
		else
		{

			retu = Scanned.Find(findleaf);
			rel = retu;
			return retu;
		}

	}

	else
	{
		Schema right;
		catalog->GetSchema(rot->rightChild->tables[0], right);
/*
		cout << rot->rightChild->tables[0] << "RIGHT CHILD" << endl;
		cout << rot->tables[0] << " " << rot->tables[1] << "CURRENT ROOT" << endl;
		cout << rot->leftChild->tables[0] << "LEFT CHILD" << endl;


		cout << rot->rightChild->tables.size() << " NUMBER OF TABLES IN RIGHT" << endl;
		cout << rot->tables.size() << " NUMBER OF TABLES IN CURRENT" << endl;
		cout << rot->leftChild->tables.size() << " NUMBER OF TABLES IN LEFT" << endl;

		cout << rot->rightChild->noTuples << " TUPLE RIGHT" << endl;
		cout << rot->noTuples << " TUPLE CURRENT" << endl;
		cout << rot->leftChild->noTuples << "TUPLE LEFT" << endl;
		*/

		Schema end;
		CNF compare;
		RelationalOp * rightrel;
		RelationalOp * leftrel = rel;
		rightrel = Build(rot->rightChild, retu, _predicate);

		//float right = rot->rightChild.


		Schema left;
		AndList  temp = *_predicate;
		for (int i = 0; i < rot->tables.size() - 1; i++) {

			Schema check;
			catalog->GetSchema(rot->tables[i], check);
			//	compare.ExtractCNF(temp, right, check);
	//			if (compare.numAnds > 0) {
			left.Append(check);
			//		}

		}

		cout << "left " << left << endl;
		cout << "right " << right << endl;

		//	cout << *leftrel << endl;
		//	cout << *rightrel<<endl;



		compare.ExtractCNF(temp, left, right);
		//	cout <<"compare joins IN JOIN " <<compare.numAnds << endl;


		RelationalOp * join;

		Schema templeft = left;
		Schema tempright = right;


		vector<int> removeLeft;
		CNF check;
		check.ExtractCNF(temp, templeft, tempright);
		cout << "FIRST CHECK" << check.ExtractCNF(temp, templeft, tempright) << endl;
		cout << check.numAnds << endl;
		for (int i = 0; i < check.numAnds; i++) {
			removeLeft.push_back(check.andList[i].whichAtt1);

		}


		vector<int> removeRight;
		for (int j = 0; j < check.numAnds; j++) {
			removeRight.push_back(check.andList[j].whichAtt2);
		}
		tempright.Project(removeRight);
		//	cout <<"PROJECTED" <<tempRight << endl;
		templeft.Project(removeLeft);

		cout << templeft << " LEFT PROJECT" << endl;

		cout << tempright << " right PROJECT" << endl;

		CNF tempCheck;
		tempCheck.ExtractCNF(temp, templeft, tempright);

		cout << tempCheck.ExtractCNF(temp, templeft, tempright) << endl;
		cout << tempCheck.numAnds << endl;
		AndList tempANDLIST = *_predicate;

		join = new Join(left, right, end, check, leftrel, rightrel, tempANDLIST);




		//cout << end << endl;
		/*
		if (Join* a = dynamic_cast<Join*>(leftrel)) { //left and right are in the right order
			join = new Join(left, right, end, compare, leftrel, rightrel, tempANDLIST);
		}
		else if (tempCheck.numAnds > 0) {
			join = new Join(left, right, end, compare, leftrel, rightrel, tempANDLIST);
		}
		else {

			cout << "change" << endl;
			CNF compare2;
			AndList  temp2 = *_predicate;
			compare2.ExtractCNF(temp2,right, left);
			join = new Join(right, left, end, compare2, rightrel, leftrel, tempANDLIST);
			//join = new Join(left, right, end, compare2, leftrel, rightrel, tempANDLIST);
			//	join = new Join(right, left, end, compare2, leftrel,rightrel);
				//Join* joining =new Join(right, left, end, compare, rel, rightrel);
		}

		*/
		rel = join;

		if (rot->parent != NULL)
			return Build(rot->parent, rel, _predicate);
		else
		{
			for (int i = 0; i < rot->tables.size(); i++) {
				cout << rot->tables[i] << " ";
			}
			cout << endl;
			return join;
		}
	}
}
	//	return retu;}




//return relop* (opp root)
// if theres no left child and right child return select or scan
//else call left and right join




























void QueryCompiler::FINDTHISSHIT(FuncOperator* parseTree, Schema& mySchema, vector<string> &a) {
	// zero out the list of operations
	int numOps = 0;
	int returnsInt;
	// now recursively build the list
	RecursivelyFINDTHISSHIT(parseTree, mySchema,a);

	// remember if we get back an integer or if we get a double
	//if (resType == Integer)	returnsInt = 1;
	//else returnsInt = 0;
}

void QueryCompiler::RecursivelyFINDTHISSHIT(FuncOperator* parseTree, Schema& mySchema, vector<string> &a) {
	// different cases; in the first case, simple, unary operation
	if ((parseTree->leftOperator == NULL) && (parseTree->right == NULL)) {
		// we have either a literal value or a variable value, so do a push

		// there are two sub-cases
		// in the first case, the value is from the record that we are operating
		// over, so we will find it in the schema
		if (parseTree->leftOperand->code == NAME) {
			// first, make sure that the attribute is there

			string s(parseTree->leftOperand->value);
			int myNum = mySchema.Index(s);
			if (myNum == -1) {
				//cerr << "ERROR: Attribute " << s << " in function not found! " << "AYYYLMAO"<< endl;
				a.push_back(s);
				//return Integer;
			}


		}
	}
	else {
		// last is to deal with an arithmetic operator

		// so first, we recursively handle the left; this should give us the left
		// side's value, sitting on top of the stack
		RecursivelyFINDTHISSHIT(parseTree->leftOperator, mySchema, a);

		// now we recursively handle the right
		RecursivelyFINDTHISSHIT(parseTree->right, mySchema, a);

		// the two values to be operated over are sitting on the stack.
		// So next we do the operation.
		// But there are potentially some typing issues.
		// If both are integers, then we do an integer operation
	
		}
}