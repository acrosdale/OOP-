#ifndef DBFILE_H
#define DBFILE_H

#include <string>

#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"

#include<stdio.h>
#include<iostream>
#include<map>
using namespace std;

class DBFile {
private:
	File file;

	string fileName;

	bool UnaddedpageExist;

	//utilize by the getnext function
	Page tempPage;

	//resposible for bulk loads
	Page loadpages;

	//resposible for holding incoming recs
	Page appendpages;

	//temp rec
	Record Rec;

	off_t _PageNum;


public:
	DBFile();
	virtual ~DBFile();
	DBFile(const DBFile& _copyMe);
	DBFile& operator=(const DBFile& _copyMe);

	int Create(char* fpath, FileType file_type);
	int Open(const char* fpath);
	int Close();

	void Load(Schema& _schema, char* textFile);

	void MoveFirst();
	void AppendRecord(Record& _addMe);
	int GetNext(Record& _fetchMe);
	bool CreateIndex(Schema tree, Schema index, string attindex, string indexfile, unsigned int tup);
	void SetName(string name);
	string GetName();


};




#endif //DBFILE_H
