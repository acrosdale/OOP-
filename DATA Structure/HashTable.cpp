#include<iostream>
#include<vector>
using namespace std;

/*
    This HashTable is implemented using chaining (collision resolution)
    and a MAX_SIZE (vertical) of 5. This data structure support the basic
    function of INSERT,DELETE,SEARCH,PRINT

*/




struct node{
    int data;
    node * next;
};

class HashTable{

    public:
        HashTable();
        ~HashTable();

        void insert (int key);
        void deleteNode (int key);
        void search (int key);
        void print();
        int getMod(int key){    return key % MAX_SIZE; }


    private:
        node** HTable;
        int MAX_SIZE;


};

void HashTable:: print(){
    //Print ALL value that exit tin the HashTable by:
        //retrieving the linked list located at each level and printing it


    for(int i = 0; i < MAX_SIZE; i++){

        node*temp = HTable[i];

        while (temp != NULL){

            std::cout <<  temp->data << " ";
            temp = temp->next;
        }
        std::cout  << '\n';
    }

}

HashTable::~HashTable(){}

HashTable::HashTable(){
    //this intialize the MAX_SIZE to 5 which the total vertical length

    MAX_SIZE = 5;
    HTable = new node*[MAX_SIZE];

    for(int i = 0; i < MAX_SIZE; i++){

        HTable[i] = NULL;
    }


}
void HashTable::insert (int key){

    //HTable[getMod(key)]
    int mod = getMod(key);

    if( HTable[mod] == NULL){
        //if the mod location is empty at new node with key

        HTable[mod] = new node;
        HTable[mod]->data = key;
        HTable[mod]->next = NULL;
    }

    else{

        //if the location is occupied go to the end of the chain and insert new nodeW/ vaules

        node * temp = HTable[mod];

        while(temp->next != NULL){

            temp = temp->next;

        }
        temp->next = new node;
        temp->next->data = key;
        temp->next->next = NULL;
    }
}

void HashTable::deleteNode (int key){

    int mod = getMod(key);

    node* temp = HTable[mod]; // get a pointer to the chain containing the value
    node * toDelete;

    if(temp->data == key){ // if the val is @ index 0

        if(temp->next != NULL){
            //store the node in a temp var, increment pointer, and delete temp pointer

            toDelete = temp;
            temp = temp->next;
            //HTable[mod] =temp;
            delete toDelete;

        }
        else{//if its the last node just delete it

            delete temp;
        }
    }

    else{

        while(temp->next->data != key){

            temp = temp->next;

        }

        toDelete = temp->next; // store the node containing the key

        if(temp->next->next != NULL){

            temp->next = temp->next->next;

        }

        delete toDelete;

    }

}

void HashTable::search (int key){
    //this fuction take the key run it throught the mod function and
    //uses the resultant mod to find the value in the HashTable by
    //finding the level(mod) then increment throught the chain to find value

    int mod = getMod(key);

    node * temp = HTable[mod];

    if(temp->data == key){

        std::cout << "KEY "<< temp->data << " FOUND @ index " << mod <<'\n';
        return;
    }

    while(temp->next != NULL){


        if(temp->next->data == key){
            std::cout << "KEY "<< temp->next->data << " FOUND @ index " << mod <<'\n';

            return;
        }

        temp = temp->next;
    }

}


int main(){

    HashTable HT;


    HT.insert(5);
    HT.insert(10);
    HT.insert(15);

    HT.print();
    HT.deleteNode(5);
    HT.print();
    HT.search(10);
    HT.search(15);


}
