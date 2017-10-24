#ifndef _REL_OP_H
#define _REL_OP_H

#include <iostream>
#include <unordered_map>
#include "Schema.h"
#include "Record.h"
#include "DBFile.h"
#include "Function.h"
#include "Comparison.h"
#include "EfficientMap.cc"
#include <stdio.h>
#include <string.h>
#include <map>
#include <list>
#include <vector>



using namespace std;




class RelationalOp {
protected:
	// the number of pages that can be used by the operator in execution
	int noPages;
public:
	// empty constructor & destructor



	RelationalOp() : noPages(-1) {}
	virtual ~RelationalOp() {}
	unsigned int GetNoPages() { return noPages; }
	// set the number of pages the operator can use
	void SetNoPages(int _noPages) { noPages = _noPages; }

	// every operator has to implement this method
	virtual bool GetNext(Record& _record) = 0;

	/* Virtual function for polymorphic printing using operator<<.
	* Each operator has to implement its specific version of print.
	*/
	virtual ostream& print(ostream& _os) = 0;

	/* Overload operator<< for printing.
	*/
	friend ostream& operator<<(ostream& _os, RelationalOp& _op);
};

class ScanIndex : public RelationalOp {
private:
	// schema of records in operator
	Schema schema, toSendSch;

	// selection predicate in conjunctive normal form
	CNF predicate;
	// constant values for attributes in predicate
	Record constants;

	string table, attribute, key;
	DBFile index, header;
	

	bool doOnce;
	Schema LeafSch, InternalSch;
	vector <Record> List;
	int listCount;


	/*SCHEMA TO SEARCH INDEX FILE
	SCHEMA PAGE INT
	SCHEMA RECORD INT
	
	
	*/

public:
	ScanIndex(Schema& _schema, Schema& _toSendSch, DBFile& _index, DBFile& _header,
	CNF& _predicate, Record& _constants, string& _table, string& _attribute );

	virtual ~ScanIndex();

	virtual void returnSchema(Schema & _schema) { _schema = schema; }

	virtual bool GetNext(Record& _record);

	virtual ostream& print(ostream& _os);
};






class Scan : public RelationalOp {
private:
	// schema of records in operator
	Schema schema;

	// physical file where data to be scanned are stored
	DBFile file;

public:
	Scan(Schema& _schema, DBFile& _file);
	virtual ~Scan();

	virtual bool GetNext(Record& _record); /*{
										   if (file.GetNext(_record)) {
										   return true;
										   }
										   else {
										   return false;
										   }
										   }*/

	virtual ostream& print(ostream& _os);
};

class Select : public RelationalOp {
private:
	// schema of records in operator
	Schema schema;

	// selection predicate in conjunctive normal form
	CNF predicate;
	// constant values for attributes in predicate
	Record constants;

	// operator generating data
	RelationalOp* producer;

public:


	Select(Schema& _schema, CNF& _predicate, Record& _constants,
		RelationalOp* _producer);
	virtual ~Select();

	virtual bool GetNext(Record& _record);/* {
										  Record rec;
										  while (producer->GetNext(rec) == true){
										  if(predicate.Run(rec,constants) == true){
										  _record = rec;
										  return true;
										  }
										  }
										  return false;
										  }*/

	virtual ostream& print(ostream& _os);
};

class Project : public RelationalOp {
private:
	// schema of records input to operator
	Schema schemaIn;
	// schema of records output by operator
	Schema schemaOut;

	// number of attributes in input records
	int numAttsInput;
	// number of attributes in output records
	int numAttsOutput;
	// index of records from input to keep in output
	// size given by numAttsOutput
	int* keepMe;

	// operator generating data
	RelationalOp* producer;

public:
	Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
		int _numAttsOutput, int* _keepMe, RelationalOp* _producer);
	virtual ~Project();


	virtual bool GetNext(Record& _record);/* {
										  //first, call nextRecord from producer. true on success
										  cout << "PROJECT GET NEXT" << endl;
										  if (producer->GetNext(_record)) {
										  // success, continue
										  // Project (int* attsToKeep, int numAttsToKeep, int numAttsNow)

										  _record.Project(keepMe, numAttsOutput, numAttsInput);
										  return true;

										  }
										  else
										  {
										  //error
										  cout << "ERROR" << endl;
										  return false;
										  }
										  }*/

	virtual ostream& print(ostream& _os);
};

class Join : public RelationalOp {
private:
	// schema of records in left operand
	Schema schemaLeft;
	// schema of records in right operand
	Schema schemaRight;
	// schema of records output by operator
	Schema schemaOut;

	// selection predicate in conjunctive normal form
	CNF predicate;

	// operators generating data
	RelationalOp* left;
	RelationalOp* right;


	OrderMaker* _omR;
	OrderMaker* _omL;
	Record temp;



	multimap<Record, int> Lrec;
	multimap<Record, int> Rrec;

	multimap<Record, string> rightbucket;
	multimap<Record, string> leftbucket;

	Record abs_left_min;
	vector<Record> left_mins;
	Record abs_right_min;
	vector<Record> right_mins;

	vector<Record>::iterator left_mins_it;
	vector<Record>::iterator right_mins_it;



	int abs_min_index;

	bool leftSmaller;
	bool join;
	bool chunks_filled;
	int R_PageSizeInBytes;
	int L_PageSizeInBytes;


	vector< DBFile > L_files;
	vector< DBFile > R_files;
	DBFile Blank;

	vector< multimap<Record, int> > l_chunks;
	vector< multimap<Record, int> > r_chunks;

	bool L_Mex;
	bool R_Mex;
	int left_sublist_count;
	int right_sublist_count;

	bool option1; //both in memory
	bool option2;// left in right out

	bool JoinReady;

		multimap<Record, int>::iterator L_it;
		multimap<Record, int>::iterator R_it;

		vector<DBFile>::iterator V_L_it;
		vector<DBFile>::iterator V_R_it;

	int whichFile;

	bool once ;
	bool once2 ;
	bool TOBE;
	bool notagain;



	bool firstrun;
	//string Path, _Path;


public://
	Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
		CNF& _predicate, RelationalOp* _left, RelationalOp* _right, AndList& _parseTree);
	virtual ~Join();

	void Create_col(int left_size, int right_size);
	void GetNextRow(string index,int whichTableOP, Record& min);
	void LoadUP(bool leftSmaller);
	bool RUNNER(int option,Record& _record);

	virtual bool GetNext(Record& _record);
	//virtual Schema GetSchema() { return schemaOut; }


	virtual ostream& print(ostream& _os);
};

class DuplicateRemoval : public RelationalOp {
private:
	// schema of records in operator
	Schema schema;

	// operator generating data
	RelationalOp* producer;

	EfficientMap <KeyInt, KeyInt > treasureMap; //this is broke af

	EfficientMap <KeyString, KeyInt > treasureMap2;

public:
	DuplicateRemoval(Schema& _schema, RelationalOp* _producer);
	virtual ~DuplicateRemoval();

	virtual bool GetNext(Record& _record);

	virtual ostream& print(ostream& _os);
};





class Sum : public RelationalOp {
private:
	// schema of records input to operator
	Schema schemaIn;
	// schema of records output by operator
	Schema schemaOut;

	// function to compute
	Function compute;

	// operator generating data
	RelationalOp* producer;

public:
	Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
		RelationalOp* _producer);
	virtual ~Sum();

	virtual bool GetNext(Record& _record);

	virtual ostream& print(ostream& _os);
};


class GroupRecc {
public:
	Record Rec;
	int intResult;
	double doubleResult;
	GroupRecc(Record recc, int intR, double doubleR);
	~GroupRecc();

};


class GroupBy : public RelationalOp {
private:
	// schema of records input to operator
	Schema schemaIn;
	// schema of records output by operator
	Schema schemaOut;

	// grouping attributes
	OrderMaker groupingAtts;
	// function to compute
	Function compute;

	// operator generating data
	RelationalOp* producer;
	EfficientMap<KeyString, KeyString> AttributedString;

	EfficientMap<KeyString, KeyDouble> AttributedDouble;
	EfficientMap<KeyString, KeyInt> AttributedInt;

	unordered_map<string, GroupRecc*> RecMap;

	unordered_map<string, GroupRecc*>::iterator it;

	EfficientMap<KeyString, Keyify<GroupRecc*> > RecordMap;

	bool ran = false;

public:
	GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
		Function& _compute, RelationalOp* _producer);
	virtual ~GroupBy();

	virtual bool GetNext(Record& _record);

	bool spitRecord(Record& _record);


	virtual ostream& print(ostream& _os);
};

class WriteOut : public RelationalOp {
private:
	// schema of records in operator
	Schema schema;

	// output file where to write the result records
	string outFile;

	// operator generating data
	RelationalOp* producer;

public:
	WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer);
	virtual ~WriteOut();

	virtual bool GetNext(Record& _record);

	virtual ostream& print(ostream& _os);
};


class QueryExecutionTree {
private:
	RelationalOp* root;

public:

	unsigned int QPage;
	void SetMem(unsigned int i);

	QueryExecutionTree() {}
	virtual ~QueryExecutionTree() {}

	void ExecuteQuery();
	void SetRoot(RelationalOp& _root) { root = &_root; }

	friend ostream& operator<<(ostream& _os, QueryExecutionTree& _op);
};

#endif //_REL_OP_H
