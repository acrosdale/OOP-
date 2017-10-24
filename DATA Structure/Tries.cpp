#include<iostream>
#include<vector>
using namespace std;
#include <unordered_map>





struct node{

    char letter;
    bool isWord; //marker for end of word. this is last node that make a word
    node * parent;
    unordered_map< char,node*  > children;

    node * findChild(char character){

        unordered_map< char,node* >::iterator it = children.find(character);

        if(it != children.end()) return it->second;

        return NULL;
    }

    void AddChild(node* child) {
        //add new character into children
        // if letter exist it wont be added
        children.insert( pair<char, node*>(child->letter,child) );

    }

    //unordered_map<char,node*> Getchildren() { return children; }
};


class TrieTree {

    public:

        TrieTree();
        ~TrieTree();
        void insertWord(string word);
        void deleteWord(string word);
        node* searchWord(string word);


    private:
        node* root;
};

TrieTree::TrieTree(){

    root = NULL;

}
TrieTree::~TrieTree(){  /*free MEMORY*/ }



void TrieTree::insertWord(string word){
    //if(word.length() < 2) return;

    if(root == NULL){

        root = new node;
        root->letter = ' ';
    }

    node * currentPtr = root;

    for(int i = 0 ; i < word.length(); i++){
        node* temp =  NULL;

        if(i == 0){ //automatically add the first letter if exist won be added

            temp = new node;
            temp->letter = word[i];
            temp->isWord = false;
            temp->parent = currentPtr; // this could null cause the root as no parent. now the root_parent = root
            currentPtr->AddChild(temp);

            i++;
        }

        temp = currentPtr->findChild(word[i-1]);
        currentPtr = temp;  // currentPtr now point to the node of the prev letter
        node* addMe = new node;

        if(i == word.length() - 1){
            addMe->isWord = true;
        }
        else{
            addMe->isWord = false;
        }

        addMe->letter = word[i];
        addMe->parent = currentPtr;
        currentPtr->AddChild(addMe);

    }
}

void TrieTree::deleteWord(string word){

    node * currentPtr = root;

    for(int i = 0 ; i < word.length(); i++){

        if( currentPtr->findChild(word[i]) != NULL ){

            currentPtr = currentPtr->findChild(word[i]);
        }
        else{
             cout << "WORD DNE" << '\n';
            return;
        }
    }

    for(int i = word.length()-1 ; i > 0; i--){

        if(currentPtr->children.size() > 0  && currentPtr->isWord == true){ // this means it apart of a other words
            currentPtr->isWord = false;
            break;
        }

        if( currentPtr->children.size()  == 0) { // this means it make no other words
            //std::cout << "parent of "<< word[i] <<" is "<< currentPtr->parent->letter << '\n';
            currentPtr = currentPtr->parent;
            currentPtr->children.erase(word[i]);
        }
    }

}


node* TrieTree::searchWord(string word){

    node * currentPtr = root;

    for(int i = 0 ; i < word.length(); i++){
        node * found = currentPtr->findChild(word[i]);

        if( found != NULL &&  found->letter == word[i] ){
            std::cout << found->letter  << '\n';
            currentPtr = found;
            continue;
        }

        if(found == NULL){
            std::cout << "WORD NOT FOUND" << '\n';
            return NULL;
        }
    }

    if(currentPtr->isWord == true){
        std::cout << "WORD FOUND" << '\n';
        return root;
    }
    else{
        std::cout << "WORD NOT FOUND" << '\n';
        return root;
    }

}


int main(){

    TrieTree tree;

    tree.insertWord("hello");
    tree.insertWord("hellos");
    tree.insertWord("leg");
    tree.insertWord("hells");
    tree.insertWord("legs");
    tree.insertWord("legging");


    tree.deleteWord("legging");
    tree.searchWord("legging");
    tree.searchWord("hello");

}
