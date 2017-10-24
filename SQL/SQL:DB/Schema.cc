#include <iostream>
#include "Config.h"
#include "Swap.h"
#include "Schema.h"
#include <map>

using namespace std;


Attribute::Attribute() : name(""), type(Name), noDistinct(0) {}

Attribute::Attribute(const Attribute& _other) :
	name(_other.name), type(_other.type), noDistinct(_other.noDistinct) {}

Attribute& Attribute::operator=(const Attribute& _other) {
	// handle self-assignment first
	if (this == &_other) return *this;

	name = _other.name;
	type = _other.type;
	noDistinct = _other.noDistinct;

	return *this;
}

void Attribute::Swap(Attribute& _other) {
	STL_SWAP(name, _other.name);
	SWAP(type, _other.type);
	SWAP(noDistinct, _other.noDistinct);
}


Schema::Schema(vector<string>& _attributes, vector<string>& _attributeTypes,
	vector<unsigned int>& _distincts) {


	for (int i = 0; i < _attributes.size(); i++) {
		Attribute a;
		a.name = _attributes[i];
		a.noDistinct = _distincts[i];
		if (_attributeTypes[i] == "INTEGER") a.type = Integer;
		else if (_attributeTypes[i] == "FLOAT") a.type = Float;
		else if (_attributeTypes[i] == "STRING") a.type = String;

		atts.push_back(a);
	}
}

Schema::Schema(const Schema& _other) {
	for (int i = 0; i < _other.atts.size(); i++) {
		Attribute a; a = _other.atts[i];
		atts.push_back(a);
	}
}

Schema& Schema::operator=(const Schema& _other) {
	// handle self-assignment first
	if (this == &_other) return *this;

	for (int i = 0; i < _other.atts.size(); i++) {
		Attribute a; a = _other.atts[i];
		atts.push_back(a);
	}

	return *this;
}

void Schema::Swap(Schema& _other) {
	atts.swap(_other.atts);
}

int Schema::Append(Schema& _other) {
	for (int i = 0; i < _other.atts.size(); i++) {
		int pos = Index(_other.atts[i].name);
		if (pos != -1) return -1;
	}

	for (int i = 0; i < _other.atts.size(); i++) {
		Attribute a; a = _other.atts[i];
		atts.push_back(a);
	}

	return 0;
}

int Schema::Index(string& _attName) {
	for (int i = 0; i < atts.size(); i++) {
		if (_attName == atts[i].name) return i;
	}

	// if we made it here, the attribute was not found
	return -1;
}

Type Schema::FindType(string& _attName) {
	int pos = Index(_attName);
	if (pos == -1) return Integer;

	return atts[pos].type;
}

int Schema::GetDistincts(string& _attName) {
	int pos = Index(_attName);
	if (pos == -1) return -1;

	return atts[pos].noDistinct;
}

//vector< pair<string, unsigned int> > NumTup;
multimap<string, unsigned int> NumTup;
bool once = false;

unsigned int Schema::GetMaxTuples(Schema& testMe) {


	if (once == false) {
		NumTup.insert(pair<string, unsigned int>("s", 10000));
		NumTup.insert(pair<string, unsigned int>("r", 5));
		NumTup.insert(pair<string, unsigned int>("ps", 800000));
		NumTup.insert(pair<string, unsigned int>("p", 200000));
		NumTup.insert(pair<string, unsigned int>("o", 1500000));
		NumTup.insert(pair<string, unsigned int>("n", 25));
		NumTup.insert(pair<string, unsigned int>("l", 6001215));
		NumTup.insert(pair<string, unsigned int>("c", 150000));

		once = true;
	}

	vector<Attribute> ourATTS = testMe.GetAtts();

	int i = 0;
	string temp = "";
	string temp2 = "";
	int k = 0;
	int count = 1;
	multimap<string, unsigned int>::iterator it1 ;
	multimap<string, unsigned int>::iterator it2 ;

	while (i < (ourATTS[0].name).length()) {

		if (ourATTS[0].name[i] == '_') {


			while (count <  ourATTS.size()) {

				while (k < temp.length()) {

					temp2 += ourATTS[count].name[k];

					k++;
				}

				if (temp.compare(temp2) != 0) {

					 it1 = NumTup.find(temp);
					 it2 = NumTup.find(temp2);

					if ((*it1).second > (*it2).second) {

						return (*it2).second;
					}
					else {
						return (*it1).second;
					}

				}

				k = 0;
				count++;
			}

		}
		else {
			temp += ourATTS[0].name[i];
		}
		i++;
	}

	it1 = (NumTup.find(temp));

	return (*it1).second;

}

int Schema::RenameAtt(string& _oldName, string& _newName) {
	int pos = Index(_newName);
	if (pos != -1) return -1;

	pos = Index(_oldName);
	if (pos == -1) return -1;


	atts[pos].name = _newName;

	return 0;
}

int Schema::Project(vector<int>& _attsToKeep) {
	int numAttsToKeep = _attsToKeep.size();
	int numAtts = atts.size();

	// too many attributes to keep
	if (numAttsToKeep > numAtts) return -1;

	vector<Attribute> copy; atts.swap(copy);

	for (int i = 0; i<numAttsToKeep; i++) {
		int index = _attsToKeep[i];
		if ((index >= 0) && (index < numAtts)) {
			Attribute a; a = copy[index];
			atts.push_back(a);
		}
		else {
			atts.swap(copy);
			copy.clear();

			return -1;
		}
	}

	copy.clear();

	return 0;
}

ostream& operator<<(ostream& _os, Schema& _c) {
	_os << "(";
	for (int i = 0; i<_c.atts.size(); i++) {
		_os << _c.atts[i].name << ':';

		switch (_c.atts[i].type) {
		case Integer:
			_os << "INTEGER";
			break;
		case Float:
			cout << "FLOAT";
			break;
		case String:
			cout << "STRING";
			break;
		default:
			cout << "UNKNOWN";
			break;
		}

		_os << " [" << _c.atts[i].noDistinct << "]";
		if (i < _c.atts.size() - 1) _os << ", ";
	}
	_os << ")";

	return _os;
}
