#include <iostream>
#include <string>
#include <stdio.h>

#include "Catalog.h"
extern "C" {
#include "QueryParser.h"
}
#include "QueryOptimizer.h"
#include "QueryCompiler.h"
#include "RelOp.h"
#include "DBFile.h"
#include "Schema.h"

using namespace std;


// these data structures hold the result of the parsing
extern struct FuncOperator* finalFunction; // the aggregate function
extern struct TableList* tables; // the list of tables in the query
extern struct AndList* predicate; // the predicate in WHERE
extern struct NameList* groupingAtts; // grouping attributes
extern struct NameList* attsToSelect; // the attributes in SELECT
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query
extern  struct TableList* indexname;
extern  struct TableList* filename;
extern struct TableList* createtable;


extern "C" int yyparse();
extern "C" int yylex_destroy();


int main() {
	// this is the catalog
	cout << "START";
	string dbFile = "catalog.db";
	Catalog catalog(dbFile);

	// this is the query optimizer
	// it is not invoked directly but rather passed to the query compiler
	QueryOptimizer optimizer(catalog);

	// this is the query compiler
	// it includes the catalog and the query optimizer
	QueryCompiler compiler(catalog, optimizer);
	/* here*/

	//void GetTables(vector<string>& _tables);
	//this catalog function return all tables
	vector<string> Tables;
	catalog.GetTables(Tables);


	// for each table
		// create heap file, set file location in catalog
		// fetch schema
		// set up char/string/w/ever as "~/Desktop/AZUREDB/code/Tables/[INSERT TABLE NAME HERE].tbl"
		 	// aka, we assume that all table text files will be there, and named after the table named
		// pass the above two into Load, ie Load(schema, textfilestring) - elly

	// this loop loads the data into the Heapfiles

	/*
	for(int i = 0; i < Tables.size(); i++){

		DBFile myFile;
		Schema tempSchema;
		string FilePathH; // the file path to heapfile.  it is in DB
		string FilePathT; // file path to .tbl files in is in table files

		if (Tables[i] != "orders")
			continue;
		char * tempName = "Tables/";
		FilePathT = tempName + Tables[i] + ".tbl";

	//	cout<< "Fetching HEAP location...."<<endl;
		catalog.GetDataFile(Tables[i], FilePathH);

		//cout<< "Creating Heap file...."<<endl;
		myFile.Create(const_cast<char*>(FilePathH.c_str()), FileType::Heap);

	//	cout<< "Opening Heap file....."<<endl;
		myFile.Open(FilePathH.c_str());

		catalog.GetSchema(Tables[i], tempSchema);
//		cout << tempSchema << endl;

		cout<< "Load Heap file....."<<endl;
		myFile.Load(tempSchema,const_cast<char*>(FilePathT.c_str()));

		cout<< "Load Done. Closing Heap file....."<<endl;
		myFile.Close();

	}*/

	int parse = -1;
	string exit = "exit";
	string s;
	// the query parser is accessed directly through yyparse
	// this populates the extern data structures
	//s = (yylval.actualChars);
	
	while (true) {
		cout << "START"<<endl;
		yylex_destroy();
		if (yyparse() == 0) {
			if (indexname != NULL) {
				cout << "DID THIS INDEX" << endl;
			
			
				DBFile HeapIndex;

				//CREATE INDEX CUST TABLE customer ON c_custkey
				//CREATE INDEX IND TABLE region ON r_regionkey
				//CREATE INDEX INDD TABLE nation ON n_nationkey
				//CREATE INDEX suppe TABLE supplier ON s_suppkey
				//CREATE INDEX PS TABLE partsupp ON ps_partkey
				//CREATE INDEX ppart TABLE part ON p_partkey
				//CREATE INDEX ooo TABLE orders ON o_orderkey
				//CREATE INDEX ooo TABLE orders ON poop
				//CREATE INDEX line TABLE lineitem on 

				string finder = tables->tableName;
				string attfind = attsToSelect->name;
				Schema temp;
				if (!catalog.GetSchema(finder, temp)) {
					cout << "NO SCHEMA ON INDEX" << endl;
					continue;
				}
				int unsigned find;
				if (catalog.GetNoDistinct(finder, attfind, find) == false) {
					cout << "UNABLE TO FIND ATT" << endl;
					continue;
				}

				cout << find << endl;
				if (find == 4294967295) {
					cout << find << endl;
					cout << "UNABLE TO FIND ATT2" << endl;
					continue;
				}



				string finding = catalog.FindIndex(tables->tableName, attsToSelect->name);
				if (finding == "") {
					cout << "INDEX FILE DOES NOT EXISTS" << endl;
					catalog.CreateIndex(indexname->tableName, tables->tableName, attsToSelect->name);

					string fileheap;
					string tableindexison = tables->tableName;
					catalog.GetDataFile(tableindexison, fileheap);

					Schema schemaIndex;
					catalog.GetSchema(tableindexison, schemaIndex);

					unsigned int tuples;
					catalog.GetNoTuples(tableindexison, tuples);


					//	catalog.Save();

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

					Schema toSrch(AttToSrch, TypeToSrch, DistToSrch);





					string indexfile;
					string temp = indexname->tableName;
					catalog.GetDataFile(temp, indexfile);

					//Index/indexname->tableName.Index
					//createindex(indexname->tableName, tables->tableName, attsToSelect->name, schemaIndex, toSrch, indefile 


					HeapIndex.Open(fileheap.c_str());
					//	HeapIndex.CreateIndex(toSrch, schemaIndex, attsToSelect->name, indexfile, tuples);




					cout << " before close" << endl;
					HeapIndex.Close();

					cout << "CLOSED" << endl;

					cout << "FINISHED CREATE INDEX" << endl;




					cout << " SAVED" << endl;
					continue;
				}
				break;
			}
			if (indexname == NULL && predicate == NULL && filename == NULL) {
				cout << "DID WE CREATE" << endl;
				//CREATE TABLE Persons PersonID int, LastName int, FirstName int, Address int, City int;
				int count = 0;

				vector<string> AttType;
				vector<string> AttName;

				Attribute a;
				while (attsToSelect != NULL) {
				//	cout << attsToSelect->name << endl;
					if (count % 2 == 0) {
						AttType.push_back(attsToSelect->name);
					}
					else
						AttName.push_back(attsToSelect->name);
					count++;
					attsToSelect = attsToSelect->next;
				}
				cout << AttType.size() << endl;
				cout << AttName.size() << endl;

				
				vector<string> AttName2;
				vector<string> AttType2;

				for (int i = AttType.size()-1; i >= 0; i--)
				{
					//cout << AttName[i] << " " << AttType[i] << endl;

					AttName2.push_back(AttName[i]);
					AttType2.push_back(AttType[i]);

				}
				
			

				string table = tables->tableName;

				Schema temp;
				if (!catalog.GetSchema(table, temp)) {
					cout << "MAKE NEW TABLE " << endl;

					catalog.CreateTable(table, AttName, AttType);
					catalog.Save();
					continue;
				}
				else {
					cout << "tABLE EXISTS " << endl;
					continue;
				}

				//create table
				//break;
			}
			if (filename != NULL) {
				cout << "DID WE FILE" << endl;
				//LOAD DATA nation FROM Tables/nation.tbl; 
				//LOAD DATA part FROM Tables/part.tbl

				DBFile myFile;
				Schema tempSchema;
				string FilePathH; // the file path to heapfile.  it is in DB
				string FilePathT = filename->tableName; // file path to .tbl files in is in table files
				//char * tempName = "Tables/";
				string temp = tables->tableName;
				if (catalog.GetDataFile(temp, FilePathH)) {
					//;
					//FilePathT = tempName + temp  + ".tbl";
				//	FilePathT += ".tbl";
					//break;
					myFile.Create(const_cast<char*>(FilePathH.c_str()), FileType::Heap);
					myFile.Open(FilePathH.c_str());
					catalog.GetSchema(temp, tempSchema);
					cout << "CRASHED HERE" << endl;
					myFile.Load(tempSchema, const_cast<char*>(FilePathT.c_str()));
					myFile.Close();
					continue;
				}
				else {
					cout << "TABLE NOT FOUND" << endl;
					continue;
				}
				//for loop from above
				//break;
			}

			//else if load
			//else

			//break;

			cout << "OK!" << endl;
			parse = 0;
			QueryExecutionTree queryTree;
			compiler.Compile(tables, attsToSelect, finalFunction, predicate,
				groupingAtts, distinctAtts, queryTree);


			queryTree.SetMem(100);
		//	cout << queryTree << endl;
		
	
		queryTree.ExecuteQuery();
		break;
	
		}
		else if (exit == (yylval.actualChars)) {
			cout << "EXITED" << endl;
		}
		else {
			if (string(yylval.actualChars) == exit) {
			
				break;
			}
			
			cout << "Error: Query is not correct!" << endl;
			continue;
			parse = -1;
		}
		if (parse != 0) return -1;

	}

	cout << "DONE" << endl;

	return 0;
}





/*
	int parse = -1;
	string exit = "exit";
	string s;
	// the query parser is accessed directly through yyparse
	// this populates the extern data structures
	//s = (yylval.actualChars);
	while (true) {

		yylex_destroy();
		if (yyparse() == 0) {
			cout << "OK!" << endl;
			parse = 0;
			QueryExecutionTree queryTree;
			compiler.Compile(tables, attsToSelect, finalFunction, predicate,
				groupingAtts, distinctAtts, queryTree);

			cout << queryTree << endl;
		}
		else {
			if (string(yylval.actualChars) == exit) {
				break;
			}
			cout << "Error: Query is not correct!" << endl;
			parse = -1;
		}
		if (parse != 0) return -1;

	}


	//if (parse != 0) return -1;

	// at this point we have the parse tree in the ParseTree data structures
	// we are ready to invoke the query compiler with the given query
	// the result is the execution tree built from the parse tree and optimized

	yylex_destroy();

	*/
