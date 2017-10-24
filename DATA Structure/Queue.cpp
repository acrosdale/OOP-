#include<iostream>
using namespace std;



struct node{
    int data;
    node* next;
};

class Queue{

    public:
        Queue();
        ~Queue();

        void enqueue(int value);
        void dequeue();
        int front();
        bool isEmpty();

    private:
        node* head;


};


Queue::Queue(){
    head = NULL;
}
Queue::~Queue();

int Queue::front(){

    if(head != NULL){
        std::cout << head->data << '\n';
    }
    else{
        std::cout << "Queue is empty" << '\n';
    }

}

bool Queue::isEmpty(){

    if(head == NULL){
        return true;
    }

    return false;
}


void Queue::enqueue(int value){

    if(head == NULL){

        head = new node;
        head->data = value;
        head->next = NULL;


    }
    else{
        node* temp = head;

        while(temp->next != NULL){
            temp = temp->next;
        }

        temp->next = new node;
        temp->next->data = value;
        temp->next->next = NULL;

    }
}

void Queue::dequeue(){

    if(head != NULL ){

        if(head->next == NULL){
            delete head;
            head = NULL;

        }
        else{

            node* temp = head;
            head = head->next;
            delete temp;
        }
    }

}



int main (){

    Queue queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
   queue.enqueue(4);
   queue.enqueue(5);

   queue.front();
   queue.dequeue();
   queue.front();

   queue.dequeue();
   queue.front();
   queue.dequeue();
    queue.front();
    queue.dequeue();
    queue.front();
    queue.dequeue();
    queue.front();


   return 0;
}
