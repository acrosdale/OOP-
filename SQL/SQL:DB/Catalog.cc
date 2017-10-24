#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <queue>
#include <functional>

#define _OPEN_SYS_ITOA_EXT
#include <set>
#include <algorithm>

#include "sqlite3.h"

#include "Schema.h"
#include "Catalog.h"

#include "EfficientMap.cc"


#include "InefficientMap.cc"
#include "TwoWayList.cc"


using namespace std;
typedef int(*sqlite3_callback)(
	void*,    /* Data provided in the 4th argument of sqlite3_exec() */
	int,      /* The number of columns in row */
	char**,   /* An array of strings representing fields in the row */
	char**    /* An array of strings representing column names */
	);

class TableAtts {
public:
	KeyString name;
	KeyInt tupple;
	KeyString filename;
	TableAtts() {};
	TableAtts(string tname, int tupp, string fname);
	string getName();


};
TableAtts::TableAtts(string tname, int tupp, string fname) {
	name = tname;
	tupple = tupp;
	filename = fname;
}
string TableAtts::getName() {
	return name;
}

vector<string> sql;
sqlite3 *db;
int AttTd;

int maxTableID = 0;
bool isTableImport = true;

int dothis = 0;
EfficientMap<KeyInt, Schema> CatalogAtt;
//EfficientMap <KeyInt, vector<int> > TAIds;
//EfficientMap <KeyInt, KeyInt> TAIds;


EfficientMap <KeyInt, KeyString > CatalogTabName;
EfficientMap <KeyInt, KeyInt > CatalogTabTup;
EfficientMap <KeyInt, KeyString> CatalogTabFName;

EfficientMap < KeyString, Keyify<vector<string> >  >Indexes;



vector<string> attributes;
vector<string> attributeTypes;
vector<unsigned int> distincts;
KeyInt currTableId = -1;


//EfficientMap <Keyify<int>, TableAtts > CatalogTab;

static int callback(void *dataa, int argc, char **argv, char **azColName) {

	//Table
	//id name tuppleamount filename     TableName AttName
	if (dothis == 0) {
		if (isTableImport) {
			KeyInt id = atoi(argv[0]);
			if (id > maxTableID) {
				maxTableID = id;    // this is for creating new tables later. shhh
			}
			string names = argv[1];
			KeyString name = names;
		
			KeyInt tupple = atoi(argv[2]);
			string  fnames = argv[3];
			KeyString fname = fnames;
			KeyInt id2 = id;//NEED THIS OR SEG FAULT
			KeyInt id3 = id2;
			CatalogTabName.Insert(id, name);
			CatalogTabTup.Insert(id2, tupple);
			CatalogTabFName.Insert(id3, fname);


		
			vector<string> temp;
			//cout << "INSERTING" << endl;
			if (argv[4] != NULL && argv[5] != NULL)
			{
		
				//id name tuppleamount filename     TableName AttName

				cout << names << endl;
				string IndexName = names;
				//Vector (AttName, IndexName)

				string tempstring = argv[4];
				KeyString IndexTable = tempstring;

				string atttemp = argv[5];

				temp.push_back(atttemp); //AttName
				temp.push_back(IndexName); //indexname
				Keyify<vector< string > >index = temp;


				cout<<IndexTable<<endl;
				KeyString tempFind = IndexTable;

				Indexes.Insert(IndexTable, index);  // TableName, vector(attname, indexname)


				
			}
		}

		//Attributes
		//Attid, Table id, AttName, AttType, Distinct
		else {
			//int attId = atoi(argv[0]);
			KeyInt tableId = atoi(argv[0]);
			string attname = argv[1];
			string atttype = (argv[2]);
			unsigned int distinctVals = atoi(argv[3]);

			//first, check if the tableId is the same as current.
			if (currTableId == tableId) {
                    // just add stuff to the list.
                attributes.push_back(attname);
				attributeTypes.push_back(atttype);
				distincts.push_back(distinctVals);

			} else {    // no longer adding to same schema!
			    if (!attributes.empty()) {
                    Schema* temp = new Schema(attributes, attributeTypes, distincts);
                    CatalogAtt.Find(currTableId).Append(*temp);
                    attributes.clear();
                    attributeTypes.clear();
                    distincts.clear();
			    }
			    currTableId = tableId;

                // next, check if there is an entry for that table already
                if (CatalogAtt.IsThere(tableId)) {

                    // if there is, great! pull up the schema associated
                    //Schema myTable = CatalogAtt.Find(tableId);
                    // next, create a new Schema to append because apparently that's what Schema wants
                    // to do this, you need vectors of each trait
                    // Schema(vector<string>& _attributes,	vector<string>& _attributeTypes, vector<unsigned int>& _distincts);

                    attributes.push_back(attname);
                    attributeTypes.push_back(atttype);
                    distincts.push_back(distinctVals);


                    //cout << myTable << endl;

                    //	cout << CatalogAtt.Find(tableId) << endl;

                }
                else {
                    // if not, whoops. gotta make a Schema, then insert it
                    //similar to if there is

                    attributes.push_back(attname);
                    attributeTypes.push_back(atttype);
                    distincts.push_back(distinctVals);
                    Schema temp(attributes, attributeTypes, distincts);
                    attributes.clear();
                    attributeTypes.clear();
                    distincts.clear();

                    // just, now you insert it into the map by table.
                    CatalogAtt.Insert(tableId, temp);

                }
            }
		}

	}
	return 0;
}




//EfficientMap<string, vector<int, string>> CatalogAtts;

Catalog::Catalog(string& _fileName) {

	char *zErrMsg = 0;
	int rc;
	string a = _fileName;
	rc = sqlite3_open(_fileName.c_str(), &db);
	//cout << _fileName << " rc " << rc << endl;

	if (!rc) {
		//cout << "guuci"<<endl;
		//cout << _fileName.c_str()<<endl;
		//sqlite3_stmt * statement;
		const char *query = "select * from tables";
		//const char * query2 = "select * from attributes";
		const char* data = "";
		//cout << SQLITE_OK<<endl;
		isTableImport = true;
		dothis = 0;

		rc = sqlite3_exec(db, query, callback, (void*)data, &zErrMsg);
		//	rc = sqlite3_exec(db, query, callback, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK) {
			sqlite3_free(zErrMsg);
		}
		else {
			//cout << "yes";
		}

		const char *query2 = "select * from attributes order by tableID";
		isTableImport = false;

		rc = sqlite3_exec(db, query2, callback, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK) {
			sqlite3_free(zErrMsg);
		}
		else {
			//cout << "yes";
		}
	}
	else {
		cout << "fail";
		cout << _fileName.c_str();
	}


	string name;
	int tupple;
	string filename;




}

Catalog::~Catalog() {
	Save();
}

bool Catalog::Save() {

	char *zErrMsg = 0;
	int rc;
	const char* data = "";
	for (int i = 0; i < sql.size(); i++) {

		const char * statement = sql[i].c_str();
		//cout << statement << endl;
		dothis = 1;
		rc = sqlite3_exec(db, statement, callback, (void*)data, &zErrMsg);
	}
	sql.clear();
	return true;
}

bool Catalog::GetNoTuples(string& _table, unsigned int& _noTuples) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		//	cout << name << endl;
		if (name == _table) {
			id = CatalogTabName.CurrentKey();

			break;
		}
	}

	if (id == 214748346) {
		return false;
	}
	else
	{
		_noTuples = CatalogTabTup.Find(id);
		return true;
	}
	return true;
}

void Catalog::SetNoTuples(string& _table, unsigned int& _noTuples) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}

	}

	if (id == 214748346) {
	}
	else
	{
		for (CatalogTabTup.MoveToStart(); !CatalogTabTup.AtEnd(); CatalogTabTup.Advance()) {
			int id2 = CatalogTabTup.CurrentKey();
			if (id == id2)
			{
				CatalogTabTup.CurrentData() = _noTuples;
				string statement = "Update tables set tupleAmount = ";
				ostringstream convert;
				convert << _noTuples;
				statement += convert.str();
				statement += " where id= ";
				ostringstream convert2;
				convert2 << id2;
				statement += convert2.str();
				statement += ";";
				sql.push_back(statement);
				break;
			}
		}
	}

}

bool Catalog::GetDataFile(string& _table, string& _path) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}

	}

	if (id == 214748346) {
		//cout << "false";
		return false;
	}
	else
	{
		_path = CatalogTabFName.Find(id);
		return true;
	}
	return true;
}

void Catalog::SetDataFile(string& _table, string& _path) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}
	}

	if (id == 214748346) {

	}
	else
	{
		for (CatalogTabFName.MoveToStart(); !CatalogTabFName.AtEnd(); CatalogTabFName.Advance()) {
			int id2 = CatalogTabFName.CurrentKey();
			if (id == id2)
			{
				CatalogTabFName.CurrentData() = _path;
				string statement = "Update tables set fileLoc = '";
				statement += _path;
				statement += "'  where id= ";
				ostringstream convert2;
				convert2 << id2;
				statement += convert2.str();
				statement += ";";
				sql.push_back(statement);
				break;
			}
		}
	}
}

bool Catalog::GetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}
	}

	if (id == 214748346 || CatalogAtt.IsThere(id) == 0) {

		return false;
	}
	else
	{
		Schema temp = CatalogAtt.Find(id);
		_noDistinct = temp.GetDistincts(_attribute);
		//cout << _noDistinct << endl;
		//cout << i << endl;
	}

	return true;
}
void Catalog::SetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {

			id = CatalogTabName.CurrentKey();

			break;
		}
	}

	if (id == 214748346) {

	}
	else
	{


		Schema temp = CatalogAtt.Find(id);
		int AttIndex = temp.Index(_attribute);
		CatalogAtt.Find(id).GetAtts()[AttIndex].noDistinct = _noDistinct; //wtf this is legal?

																		  /*
																		  Schema temp = CatalogAtt.Find(id);
																		  int AttIndex = temp.Index(_attribute);
																		  vector<Attribute> tempatts = temp.GetAtts();
																		  tempatts[AttIndex].noDistinct= _noDistinct;
																		  */

		string statement = "Update attributes set distinctValues = ";
		ostringstream convert;
		convert << _noDistinct;
		statement += convert.str();
		statement += " where tableID= ";
		ostringstream convert2;
		convert2 << id;
		statement += convert2.str();
		statement += " AND attributeName = '";
		statement += _attribute;
		statement += "';";
		sql.push_back(statement);

	}

}

void Catalog::GetTables(vector<string>& _tables) {
	vector<string> table;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		table.push_back(name);


	}
	_tables = table;

}

bool Catalog::GetAttributes(string& _table, vector<string>& _attributes) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}
	}

	if (id == 214748346 || CatalogAtt.IsThere(id) ==0) {
		//cout << "crash false" << endl;
		return false;
	}
	else
	{
		Schema temp = CatalogAtt.Find(id);
		vector<Attribute> tempAtt = temp.GetAtts();
		for (int i = 0; i < tempAtt.size(); i++) {
			_attributes.push_back(tempAtt[i].name);

		}
	}


	return true;
}

bool Catalog::GetSchema(string& _table, Schema& _schema) {
	KeyInt id = 214748346;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			id = CatalogTabName.CurrentKey();
			break;
		}
	}
	//cout << "check schema" << endl;
	if (id == 214748346 || CatalogAtt.IsThere(id)==0) {
		//cout << "crashSchema" << endl;
		return false;
	}
	else
	{
		_schema = CatalogAtt.Find(id);

	}

	return true;
}

bool Catalog::CreateTable(string& _table, vector<string>& _attributes,
	vector<string>& _attributeTypes) {


	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		if (name == _table) {
			return false;
		}
	}
	set<string> unique(_attributes.begin(), _attributes.end());
	if (unique.size() != _attributes.size()) {
		return false;
	}


	// insert shitty coding practices because i'm trying to make it more efficient and i'm awful at it

	//CREATE TABLE t(x INTEGER, y TYPE, z TYPE);
	string statement = "INSERT INTO tables (id, name, tupleAmount, fileLoc) values (";
	ostringstream convert;
	maxTableID++;
	convert << maxTableID;
	statement += convert.str() + ", ";  // insert id
	statement += "'" + _table + "', ";  // insert name
	statement += "0, ";                  // insert tupleAmount - start off empty, so 0
	statement += "'Heapfiles/" + _table + ".heap'";             // no idea what to insert for fileLoc at this point tbh, !! TODO
	statement += ");";
	sql.push_back(statement);
	// and create table in catalog.
	KeyInt id = maxTableID;
	KeyInt id2 = id;
	KeyInt id3 = id2;
	KeyInt tup = 0;
	KeyString tabname = _table;
	KeyString filename = (string)"n/a"; // again, frick if I know what to put here, !! TODO
	CatalogTabName.Insert(id, tabname);
	CatalogTabTup.Insert(id2, tup);
	CatalogTabFName.Insert(id3, filename);

	// going to assume there is the correct number of inputs. no "blank" types, etc
	int i;
	int len = _attributes.size();
	for (i = 0; i<len; i++) {  // for each attrib, we gotta make a statement.
		string statementatt = "INSERT INTO attributes (tableID, attributeName, attributeType, distinctValues) values (";
		statementatt += convert.str() + ", ";  // insert id of associated table
		statementatt += "'" + _attributes[i] + "', ";  // insert name
		statementatt += "'" + _attributeTypes[i] + "', ";  // insert type
		statementatt += "0);";  // insert distinct vals. but ofc, atm it should be empty.
		sql.push_back(statementatt);
	}
	// and now, create attrib in catalog.
	// basically like in callback import, but a lot simpler because you already have two of the vectors!
	// hence why it's outside of the loop.
	vector<unsigned int> distincts;
	for (i = 0; i<len; i++) distincts.push_back(0);
	// all we have to do is create a populate the distinct vector with 0s.
	// then we can insert all three vectors straight-up.

	KeyInt id4 = maxTableID;
	Schema temp(_attributes, _attributeTypes, distincts);
	CatalogAtt.Insert(id4, temp);

	// and we're done!


	return true;
}

bool Catalog::DropTable(string& _table) {
	// ew this isn't efficient. but -
	// first, iterate through the table map until you find one matching the string.
	CatalogTabName.MoveToStart();
	KeyInt keyID = -1;
	KeyInt dummyk; KeyString dummyd; Schema dummys; // these two are supposed to hold the removed data but we really don't care

	while (!CatalogTabName.AtEnd()) {
		if (!_table.compare(CatalogTabName.CurrentData())) {
			keyID = CatalogTabName.CurrentKey();
			break;
		}
		CatalogTabName.Advance();
	}
	if (!_table.compare(CatalogTabName.CurrentData())) {
		keyID = CatalogTabName.CurrentKey();
	}
	else
		return false;

	// then, if you found the table id, remove from all in-memory tables and add sql statements.
	if (keyID != -1) {
		KeyInt id1 = keyID;
		KeyInt id2 = id1;
		KeyInt id3 = id2;
		KeyInt id4 = id3;
		KeyInt id5 = id4;
		CatalogTabName.Remove(id1, dummyk, dummyd);

		CatalogTabTup.Remove(id2, dummyk, dummyk);

		CatalogTabFName.Remove(id3, dummyk, dummyd);

		CatalogAtt.Remove(id4, dummyk, dummys);


		ostringstream convert;
		convert << id4;
		string statementatt = "DELETE FROM attributes WHERE tableID = ";
		statementatt += convert.str();
		statementatt += ";";

		//cout << statementatt<<endl;
		sql.push_back(statementatt);
		string statement = "DELETE FROM tables WHERE id = ";
		statement += convert.str();
		statement += ";";
		sql.push_back(statement);
	}
	else {
		cout << "No such table found.\n" << endl;
	}

	return true;
}


ostream& operator<<(ostream& _os, Catalog& _c) {

	priority_queue<string, vector<string>, greater<string> > q;
	vector<int> ids;
	for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
		string name = CatalogTabName.CurrentData();
		q.push(name);
	}



	while (!q.empty()) {
		string fname;
		unsigned int tup;
		KeyInt id;
		Schema tempSch;


		string temp = q.top();

		for (CatalogTabName.MoveToStart(); !CatalogTabName.AtEnd(); CatalogTabName.Advance()) {
			string name = CatalogTabName.CurrentData();
			if (name ==  temp) {
				id = CatalogTabName.CurrentKey();
			}
		}
		KeyInt id2 = id;
		KeyInt id3 = id2;
		fname = CatalogTabFName.Find(id);
		tup = CatalogTabTup.Find(id2);

		//cout << temp << '\t' << fname << '\t' << tup << endl;
		_os << temp;
		_os << '\t';
		_os << fname;
		_os << '\t';
		_os << tup;
		_os << '\n';
		//cout << _c.GetDataFile(temp, fname) << endl;
		vector<string> a;

		if (CatalogAtt.IsThere(id3)) {
			_c.GetSchema(temp, tempSch);
			int sizeAtt = tempSch.GetNumAtts();
			priority_queue<string, vector<string>, greater<string> > q2;
			for (int j = 0; j < sizeAtt; j++) {
				ostringstream convert;
				string temp2 = tempSch.GetAtts()[j].name;
				int disttemp = tempSch.GetAtts()[j].noDistinct;
				Type t = tempSch.GetAtts()[j].type;

				string push2;
				push2 += temp2;
				push2 += '\t';


				switch (t) {
				case Integer:
					push2 += "INTEGER";
					break;
				case Float:
					push2 += "FLOAT";
					break;
				case String:
					push2 += "STRING";
					break;
				default:
					push2 += "UNKNOWN";
					break;
				}
				push2 += '\t';
				convert << disttemp;
				push2 += convert.str();
				//push2 += '\n';

				q2.push(push2);
			}

			while (!q2.empty()) {
				string temper = q2.top();
				_os << '\t';
				_os << temper;
				_os << '\n';
				q2.pop();
			}

		}

		q.pop();


	}
	return _os;

}




bool Catalog::CreateIndex(string IndexName, string TableName, string AttName) {
	// insert shitty coding practices because i'm trying to make it more efficient and i'm awful at it
	//CREATE TABLE t(x INTEGER, y TYPE, z TYPE);
	string statement = "INSERT INTO tables (id, name, tupleAmount, fileLoc, TableName, AttName) values (";
	ostringstream convert;
	maxTableID++;
	convert << maxTableID;
	statement += convert.str() + ", ";  // insert id
	statement += "'" + IndexName + "', ";  // insert name
	statement += "0, ";                  // insert tupleAmount - start off empty, so 0
	statement += "'Index/";// no idea what to insert for fileLoc at this point tbh, !! TODO
	statement += IndexName;
	statement += ".Index',";
	statement += "'" + TableName + "', ";
	statement += "'" + AttName + "'";
	statement += ");";
	sql.push_back(statement);
	// and create table in catalog.
	KeyInt id = maxTableID;
	KeyInt id2 = id;
	KeyInt id3 = id2;
	KeyInt tup = 0;
	KeyString tabname = IndexName;
	vector<string> temp;
	temp.push_back(AttName);
	temp.push_back(IndexName);
	KeyString tabname2 = IndexName;
	KeyString ActualTabName = TableName;
	KeyString Att = AttName;
	Keyify<vector<string> > att_index = temp;


	KeyString filename = (string)("Index/" + IndexName + ".Index"); // again, frick if I know what to put here, !! TODO
	CatalogTabName.Insert(id, tabname);
	CatalogTabTup.Insert(id2, tup);
	CatalogTabFName.Insert(id3, filename);

	Indexes.Insert(ActualTabName, att_index);
	return true;
}


string Catalog::FindIndex(string TableName, string AttName){

	KeyString TabName = TableName;

	cout << TableName << " " << AttName << endl;
	if (Indexes.Length() == 0) {
		return "";
	}


	if (Indexes.IsThere(TabName)) {
		
		vector<string> att_indexname = Indexes.Find(TabName);

		if (att_indexname[0] == AttName) {
			return att_indexname[1];
		}
		else
			return "";


	}
	else
		return "";
}