#include <iostream>
using namespace std;
/*
    A BST is a binary tree where nodes are ordered in the following way:
        *each node contains one key (also known as data)
        *the keys in the left subtree are less then the key in its parent node, in short L < P;
        *the keys in the right subtree are greater the key in its parent node, in short P < R;
        *duplicate keys are not allowed.

    depth-first traversals, :
        PreOrder traversal  -  visit  the (parent,left,right);
        InOrder traversal   -  visit  the (left,parent,right );
        PostOrder traversal -  visit  the (left,right,parent);

*/


struct node{
    int key;      // hold the value of each node
    node * left;  //point to the left child of the parent node
    node * right; // point to the right child of the parent node
};


class BST {

    public:

        BST();
        ~BST();
        void insert(int key_val);
        void search (int key_val);
        void destroy_BST();
        node* deleteNode(int key_val);

        int GetMinNode(node* leaf);// traverse down to find the leftmost leaf

        void PreOrder();
        void InOrder ();
        void PostOrder();

    private:
        node* root;
        node* search (int key_val, node * leaf);
        node* deleteNode(int key_val, node * leaf);
        void insert(int key_val, node * leaf);
        void destroy_BST(node * leaf);

        void PreOrder(node * leaf);
        void InOrder (node * leaf);
        void PostOrder(node * leaf);

};

BST::BST(){
    root = NULL;
}

BST::~BST(){
    destroy_BST();
}


void BST::insert(int key_val){

    if (root == NULL){ // store assume tree is empty as such we can store the first key into the root

        root        = new node;
        root->key   = key_val;

        // nullify the new  pointers
        root->left  = NULL;
        root->right = NULL;
    }
    else{ // root is establish find a place in the tree for it

        insert(key_val,root);

    }
}


void BST::insert(int key_val, node * leaf){

    if( key_val < leaf->key ){ // go left

        if(leaf->left != NULL){  // traverse down to find an empty spot
            insert(key_val,leaf->left);
        }

        if(leaf->left == NULL){ // we found a space to put the node

            leaf->left = new node; //intialize new node
            leaf->left->key = key_val; //place the key into this new node

            // nullify the new node pointers
            leaf->left->left = NULL;
            leaf->left->right = NULL;
        }

    }
    else if (key_val > leaf->key){ // go right

        if(leaf->right != NULL){
            insert(key_val,leaf->right);

        }
        if(leaf->right == NULL){

            leaf->right = new node;
            leaf->right->key = key_val;

            //nullify pointers
            leaf->right->left  = NULL;
            leaf->right->right = NULL;
        }
    }

    else{
        cout<< "key was a duplicate"<<endl;
        return;
    }
}



void BST::search (int key_val){

    search (key_val, root);

}

node* BST::search (int key_val, node * leaf){

    if(leaf->key == key_val ){ // we found the key

        cout<<"Key found"<<endl;
        return leaf;

    }

    else if( key_val < leaf->key ){ // go left

        if(leaf->left != NULL){  // traverse
            search(key_val,leaf->left);
        }

    }
    else if (key_val > leaf->key){ // go right

        if(leaf->right != NULL){
            search(key_val,leaf->right);

        }
    }

    else{
        cout<<"Key NOT found"<<endl;
        return NULL;
    }

}


void BST::destroy_BST(){

    destroy_BST(root);

}

void BST::destroy_BST(node * leaf){

    if(leaf != NULL){
        destroy_BST(leaf->left);
        destroy_BST(leaf->right);
        delete leaf;
    }

}

int BST::GetMinNode(node * leaf){

    if(leaf == NULL){

        return NULL;

    }
    node* min = leaf;

    // traverse down to find the leftmost leaf
    while (min->left != NULL){
        min = min->left;
    }

    return min->key;
}


node* BST::deleteNode(int key_val){


    deleteNode( key_val, root);

}

node* BST::deleteNode(int key_val, node * leaf){

    //cout<<"search"<<endl;
    if(leaf == NULL)  return leaf;

    //go left traverse
    if( key_val < leaf->key  && leaf->left != NULL){
        //cout<<"left"<<endl;
        leaf->left = deleteNode(key_val,leaf->left);
    }
    //go right traverse
    else if (key_val > leaf->key && leaf->right != NULL){
        //cout<<"right"<<endl;
        leaf->right = deleteNode(key_val,leaf->right);
    }

    else{ //leaf->key == key_val
        //cout<<"found"<<endl;
        // case 1: no children
        if (leaf->left == NULL && leaf->right == NULL) {
            delete leaf; // free the memory occupied by the node
            leaf = NULL; // nullify all pointer and data

        }

        // case 2: one child (right)
        else if (leaf->left == NULL && leaf->right != NULL){
            node * temp = leaf;   // the leaf pointer point to the parent to be deleted
            leaf = leaf->right;  // the parent pointer now point to the child
            delete temp; //delete parent pointer
            //temp = NULL;
        }

        // case 3: one child (left)
        else if (leaf->left != NULL && leaf->right == NULL){
            node* temp  = leaf;
            leaf = leaf->left;
            delete temp;
            //temp = NULL;
        }

        // case 4: two children
        else
        //(leaf->left != NULL && leaf->right != NULL)
        {
            // get the min node of the right subtree || we could also get the max node of the left subtree
            leaf->key = GetMinNode(leaf->right); // duplicate the min into the parent node
            leaf->right = deleteNode(leaf->key, leaf->right); //***delete the child from the subtree & return new subtree
        }

    }



    return leaf; //***this update parent node with new subtree

}




void BST::PreOrder(){
    PreOrder(root);
}
void BST::InOrder(){
    InOrder(root);
}
void BST::PostOrder(){
    PostOrder(root);
}

void BST::PreOrder(node * leaf){

    if(leaf != NULL){

        cout<< leaf->key <<" "<<endl;
        PreOrder(leaf->left);
        PreOrder(leaf->right);
    }
}
void BST::InOrder(node * leaf){

    if(leaf != NULL){

        InOrder(leaf->left);
        cout<< leaf->key <<endl;
        InOrder(leaf->right);
    }
}

void BST::PostOrder(node * leaf){

    if(leaf != NULL){

        PostOrder(leaf->left);
        PostOrder(leaf->right);
        cout<< leaf->key <<" "<<endl;
    }
}



int main(){
    BST tree;

    tree.insert(10);
    tree.insert(6);
    tree.insert(14);
    tree.insert(5);
    tree.insert(8);
    tree.insert(11);
    tree.insert(18);
    tree.deleteNode(5);
    tree.PreOrder();
    tree.search(18);
}
