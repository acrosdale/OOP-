#include <string>
#include <stdio.h>
#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "DBFile.h"


using namespace std;

#define SIZE 5


class BPTree
{

public:
	int n; // number of key
	int *keys;
	int* pageIdx; // this is suppose to an increasing 1-> ######
	int* recordIdx;
	BPTree **p_child;
	bool leaf;

	int size1;
	int size2;


}*root = NULL, *np = NULL, *x = NULL;


BPTree * BPT();
void sort(int *p, int *p2, int *p3, int n);
void split_child(BPTree *x, int i, int &Mid, int &MidPg, int &MidRec);
//void insert(int a, int pagenumber, int recordnumber);
void insert(int a, int pagenumber, int recordnumber);
//void search(int k,BPTree *p, vector< pair<off_t, int> >& indexed) ;
void traverse(BPTree *x);


DBFile::DBFile() : fileName("") {
	UnaddedpageExist = false;
	_PageNum = 0;


}

DBFile::~DBFile() {

}

void DBFile::SetName(string name) {
	fileName = name;
}
string DBFile::GetName() {
	return fileName;
}

DBFile::DBFile(const DBFile& _copyMe) :
	file(_copyMe.file), fileName(_copyMe.fileName) {}

DBFile& DBFile::operator=(const DBFile& _copyMe) {
	// handle self-assignment first
	if (this == &_copyMe) return *this;

	file = _copyMe.file;
	fileName = _copyMe.fileName;

	return *this;
}

int DBFile::Create(char* f_path, FileType f_type) {

	//Create creates a new heap file. FileType has to be Heap.
	//This is done only once, when a SQL table is created.

	if (f_type != FileType::Heap && f_type != FileType::Sorted) {// check the file type id the match Heap. AC
		cout << "WRONG FileTYPE  " << endl;
		return -1;
	}
	else {

		if (file.Open(0, f_path) == 0) { // if file is sucessfully open a new file is created. AC
			_PageNum = 0;
			file.SetCurrPage(_PageNum); //set the current page pointer
			file.Close();
			return 0;
		}
		else {
			return -1;
		}

	}
}

int DBFile::Open(const char* f_path) {
	//Open gives access to the heap file.
	//The name is taken from the catalog, for every table.

	if (file.Open(1, const_cast<char*>(f_path)) == 0) {
		//if file is sucessfully open. AC
		_PageNum = 0;
		file.SetCurrPage(_PageNum);

		return 1;
	}
	else
	{
		return 0;
	}
}



void DBFile::Load(Schema& schema, char* textFile) {
	//							    "name.txt"???? ask
	/*
	Load extracts records with a known schema from a text file passed as parameters. Essentially, it
	converts the data from text to binary. Method ExtractNextRecord from class Record does all the
	work for a given schema
	*/
	//Page loadpages;
	//Record rec;
	FILE * fp = NULL;

	fp = fopen(textFile, "r");

	if (fp != NULL) {

		while (true) {

			if (Rec.ExtractNextRecord(schema, *fp) == 1) { // if thier is DATA get bit and store in pages

				if (loadpages.Append(Rec) == 1) {
					continue;
					//Rec was added sucessfully DO NOTHING
				}
				else { // pages is full ADD IT TO FILe
					file.AddPage(loadpages, file.GetLength()); //add to file
					loadpages.EmptyItOut(); //clear in mem pages
					if (loadpages.Append(Rec))
						continue;
				}

			}
			//what if no record was added ?? did we add empty page
			else {
				//add whatever has been written to the last page
				file.AddPage(loadpages, file.GetLength());
				loadpages.EmptyItOut();  //clear in mem pages
				Rec.Nullify(); //clear in mem Rec
				break;
			}
		}
	}

	//close FILE*
	fclose(fp);

}

int DBFile::Close() {

	if (UnaddedpageExist == true) {
		file.AddPage(appendpages, file.GetLength()); // add the full page to the file
		appendpages.EmptyItOut();//clear the in memeory page

		UnaddedpageExist = false;
	}
	//tempPage.EmptyItOut();
	//	loadpages.EmptyItOut();
	Rec.Nullify();
	_PageNum = 0;

	file.SetCurrPage(_PageNum);
	tempPage.EmptyItOut();
	//utilize by the getnext function

	loadpages.EmptyItOut();




	//• Close closes the file.
	file.Close();

}

void DBFile::AppendRecord(Record& rec) {
	/*AppendRecord appends the record passed as parameter to the end of the file. This is the only method
	to add records to a heap file
	*/

	/* AC: Logic:
	* Try adding the record to the current page
	* if adding fails, write page to file and create new page
	*/

	// return 1 on success and 0 if there is no space on page
	if (appendpages.Append(rec) == 1) {
		// we good do NOTHING
	}
	else {// appendpages "cant" hold the new record. its full

		file.AddPage(appendpages, file.GetLength()); // add the full page to the file
		appendpages.EmptyItOut();//clear the in memeory page
		appendpages.Append(rec);// add new record

								/* _NOTE
								* these two are called during close as well because we might have unadded pages left.
								*/
	}

	UnaddedpageExist = true;// their now exit a page that is not in file

}

void DBFile::MoveFirst() {
	// "pointer" that points to the "current" page in the file, and to the "current" record on that page.
	//MoveFirst resets the file pointer to the BEGINNING of the file, i.e., the first record.
	//Page starts with 0, but data is stored from 1st page onwards
	_PageNum = 0;

	file.SetCurrPage(_PageNum);
}

int DBFile::GetNext(Record& rec) {
	//GetNext returns the next record in the file. The file pointer is moved to the following record
	//
	//cout << "we are here" << endl;
	//	cout << fileName << endl;
	if (UnaddedpageExist == true) {

		file.AddPage(appendpages, file.GetLength()); //we might still have un added pages
		UnaddedpageExist = false;
		appendpages.EmptyItOut();

	}

	//first time being called
	// Page starts with 0, but data is stored from 1st page onwards
	if (file.GetCurrPage() == 0) {
		_PageNum = 0;
		file.GetPage(tempPage, _PageNum); //getpage increment its value of pageNum to 1
		_PageNum++; //set the pageNum to 2
		file.SetCurrPage(_PageNum);

	}

	/*
	*get a record
	*delete current record from page and return it
	*GetFirst() return 0 if there are no records in the page, something else otherwise
	*/

	int retVal = tempPage.GetFirst(rec);
	//	cout << tempPage.GetSizeInBytes() << endl;
	//retVal = tempPage.GetFirst(rec);
	//cout <<"RETVAL"<< retVal << endl;
	//	cout << rec.GetBits() << endl;

	while (retVal == 0) { //False==0
						  //	cout << "IN WHILE GETNEXT" << endl;

		if (file.GetCurrPage() >= file.GetLength()) {
			//		cout<< "no more records left in heap to get. EOF REACHED!!"<<endl;
			return 0;
			//	b	reak;
		}

		if (file.GetCurrPage() < file.GetLength()) {
			//	cout << "passed record" << endl;
			tempPage.EmptyItOut();
			file.GetPage(tempPage, _PageNum); //getpage increment its value of pageNum to 1
			_PageNum++;
			file.SetCurrPage(_PageNum);
			retVal = tempPage.GetFirst(rec);

		}

	}

	return 1;

}


bool DBFile::CreateIndex(Schema BTree, Schema Index, string attindex, string indexfile, unsigned int tup) {


	Record fetch;

	int indexcol;


	int scale;

	float factor = tup;

	if (factor / 999999 > 1) {
		scale = 1000;  //orders  lineitem
	}
	else if (factor / 99999 > 1) {
		scale = 500;  //part customer   partsupp
	}
	else if (factor / 9999 > 1) {
		scale = 200;  //supplier

	}
	else
	{
		cout << " 5" << endl;
		scale = 5;  //nation region
	}

	indexcol = Index.Index(attindex);


	cout << BTree << endl;
	cout << Index << endl;
	cout << attindex << endl;
	cout << indexfile << endl;

	cout << indexcol << endl;


	cout << endl;
	//return false;


	int recordreset = 1;
	int reccount = 0;

	cout << "BOUT TO HIT LOOP" << endl;

	while (GetNext(fetch) == true) {

		//		cout << "LOOP"<<endl;


		//fetch.print(cout, Index);
		//	cout << endl;

		char* ptr = fetch.GetColumn(indexcol);

		int key = (*((int*)ptr));



		if (recordreset != _PageNum && _PageNum != 0)
		{

			cout << "KEY" << key << endl;
			//		cout << recordreset << endl;
			recordreset = _PageNum;
			reccount = 0;
			insert(key, recordreset, reccount);
			//		insert2(key, scale);

		}
		else if (_PageNum != 0) {
			//	cout << "AGG" << endl;
			cout << "KEY" << key << endl;
			insert(key, recordreset, reccount);
			//		insert2(key, scale);
			reccount++;
		}





	}

	Record r;

	File file;
	//	cout << indexfile << endl;
	cout << "FINISHED INSERT" << endl;


	//Record r;

	//File file;
	//	cout << indexfile << endl;

	traverse(root);


	//Schema BTree
	/*
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

	*/






	Close();
	cout << "HUH" << endl;
	return false;


}







BPTree * BPT()
{
	int i;
	np = new BPTree;
	np->keys = new int[5];
	np->pageIdx = new int[5];
	np->recordIdx = new int[5];

	np->p_child = new BPTree *[6];
	np->leaf = true;
	np->n = 0;
	for (i = 0; i < 6; i++)
	{
		np->p_child[i] = NULL;
	}
	return np;
}

void sort(int *p, int n)
{
	int i, j, temp, temp2, temp3;
	for (i = 0; i < n; i++)
	{
		for (j = i; j <= n; j++)
		{
			if (p[i] > p[j])
			{
				temp = p[i];


				p[i] = p[j];


				p[j] = temp;

			}
		}
	}
}

int split_child(BPTree *x, int i)
{
	int j, mid;
	BPTree *np1, *np3, *y;
	np3 = BPT();
	np3->leaf = true;



	if (i == -1)
	{
		mid = x->keys[2];
		x->keys[2] = 0;
		x->n--;
		np1 = BPT();
		np1->leaf = false;
		x->leaf = true;
		for (j = 3; j < 5; j++)
		{
			np3->keys[j - 3] = x->keys[j];
			np3->p_child[j - 3] = x->p_child[j];
			np3->n++;
			x->keys[j] = 0;
			x->n--;
		}
		for (j = 0; j < 6; j++)
		{
			x->p_child[j] = NULL;
		}
		np1->keys[0] = mid;
		np1->p_child[np1->n] = x;
		np1->p_child[np1->n + 1] = np3;
		np1->n++;
		root = np1;
	}
	else
	{
		y = x->p_child[i];
		mid = y->keys[2];
		y->keys[2] = 0;
		y->n--;
		for (j = 3; j < 5; j++)
		{
			np3->keys[j - 3] = y->keys[j];
			np3->n++;
			y->keys[j] = 0;
			y->n--;
		}
		x->p_child[i + 1] = y;
		x->p_child[i + 1] = np3;
	}
	return mid;
}


void traverse(BPTree *x)
{
	cout << endl;
	int i;
	for (i = 0; i < x->n; i++)
	{
		if (x->leaf == false && x->p_child != NULL)
		{

			traverse(x->p_child[i]);
		}
		cout << x->keys[i] << endl;
		//cout << " " << x->data[i];
	}
	if (x->leaf == false && x->p_child != NULL)
	{

		traverse(x->p_child[i]);
	}
	cout << endl;
}




void insert(int a, int pagenumber, int recordnumber)
{
	int i, temp;
	x = root;
	if (x == NULL)
	{
		root = BPT();
		x = root;
	}
	else
	{
		if (x->leaf == true && x->n == 5)
		{
			temp = split_child(x, -1);
			x = root;
			for (i = 0; i < (x->n); i++)
			{
				if ((a > x->keys[i]) && (a < x->keys[i + 1]))
				{
					i++;
					break;
				}
				else if (a < x->keys[0])
				{
					break;
				}
				else
				{
					continue;
				}
			}
			x = x->p_child[i];
		}
		else
		{
			while (x->leaf == false)
			{
				for (i = 0; i < (x->n); i++)
				{
					if ((a > x->keys[i]) && (a < x->keys[i + 1]))
					{
						i++;
						break;
					}
					else if (a < x->keys[0])
					{
						break;
					}
					else
					{
						continue;
					}
				}
				if ((x->p_child[i])->n == 5)
				{
					temp = split_child(x, i);
					x->keys[x->n] = temp;
					x->n++;
					continue;
				}
				else
				{
					x = x->p_child[i];
				}
			}
		}
	}
	x->keys[x->n] = a;
	sort(x->keys, x->n);
	x->n++;
}
