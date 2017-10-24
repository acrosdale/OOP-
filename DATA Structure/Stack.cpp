#include<iostream>
using namespace std;



struct node{
    int data;
    node* next;
};

class Stack{

    public:
        Stack();
        ~Stack();
        void push(int value);
        void pop();
        int top();
        bool isEmpty();

    private:
        node* head;
        node* tail;

};

Stack::Stack(){
    head = tail =NULL;
}

Stack::~Stack(){

}

void Stack::push(int value){

    if(head == NULL){
        head = new node;
        head->data = value;
        head->next = NULL;
        tail = head;
    }
    else{
        node * temp = head;
        while(temp->next != NULL){

            temp = temp->next;
        }

        temp->next = new node;
        temp->next->data = value;
        temp->next->next = NULL;
        tail = temp->next;

    }
}

void Stack::pop(){
    if(head == tail){
        delete head,tail;
        head = tail =NULL;


    }
    else if( isEmpty() == false ){

        node * temp = head;

        while(temp->next != NULL){


            if(temp->next->next == NULL){
                break;

            }
            temp = temp->next;
        }

        tail = temp;
        node * toDelete = temp->next;
        delete toDelete;
        temp->next = NULL;

    }
}
int Stack::top(){


    if(tail != NULL) cout<< tail->data <<endl;

    else{
        std::cout << "Stack is empty" << '\n';
    }


}
bool Stack::isEmpty(){

    if(head != NULL){

        return false;
    }

    std::cout << "EMPTY STACK" << '\n';
    return true;
}


int main (){

    Stack stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
   stack.push(4);
   stack.push(5);

   stack.top();
   stack.pop();
   stack.top();

   stack.pop();
   stack.top();
   stack.pop();
    stack.top();
    stack.pop();
    stack.top();
    stack.pop();
    stack.top();


   return 0;
}
