#include <iostream>
using namespace std;


struct node{
    node* next;
    int data;

};

class List{
    public:
        List();
        ~List();
        void insert(int value);
        void deleteNode(int value);
        void search(int value);
        void traverse(node * head);
        void merge(List& addMe);
        node* getHead(){
            return Head;
        }

    private:
        node* Head;
        void insert(int value, node* head);
        void deleteNode(int value,node* head);
        void search(int value,node* head);

};

List:: List(){

    Head =NULL;

}
List::~List(){

    while(Head != NULL){

        node *temp = Head;
        Head = Head->next;
        delete temp;


    }


}
void List::merge(List& addMe){

    node * head = getHead();
    while(head->next != NULL){
        head = head->next;
    }
    head->next = addMe.getHead();
    //addMe.~List();

}

void List::insert(int value){

   if(Head == NULL){

        Head  = new node;
        Head->data = value;
        Head->next = NULL;
        cout<<"data inserted "<< Head->data <<endl;

    }
    else{

        insert(value, Head);

    }
}

void List::insert (int value, node* head){

    if(head->next == NULL){

        head->next  = new node;
        head->next->data = value;
        head->next->next = NULL;
        cout<<"data inserted"<< head->next->data <<endl;

    }
    else{

        insert(value, head->next);

    }
}



void List::deleteNode(int value){

    if(value == Head->data){
        node * temp = Head;
        Head = Head->next;
        delete temp;
         cout<<"data inserted"<<endl;

    }
    else{

        deleteNode(value, Head);
    }

}

void List::deleteNode (int value,node* head){

    if(head->next->data != value){
        deleteNode(value, head->next);
    }
    else{

        node* temp = head->next;
        head->next =head->next->next;
        delete temp;

    }

}


void List::search(int value){
    search( value, Head);
}

void List::search(int value,node* head){
    if(head->data != value){
        search( value, head->next);
    }else{
        std::cout << "data found" << '\n';
    }
}


void List::traverse(node* head){

    if(head != NULL){
        std::cout <<head->data <<" "<< '\n';
        traverse(head->next);
    }
}


int main(){
    List link;
    List Link;

    link.insert(1);
    link.insert(2);
    link.insert(3);
    link.insert(4);
    link.insert(5);

    Link.insert(6);
    Link.insert(7);
    Link.insert(8);
    Link.insert(9);
    Link.insert(10);

    link.merge(Link);
    link.traverse(link.getHead());


    link.~List();
    Link.~List();



}
