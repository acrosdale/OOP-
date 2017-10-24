#include<iostream>
#include <vector>
using namespace std;


//this code represent a min HEAP
//Where the root node is less than or equal to either of its children.
    /*
        PARENT      (index-2)/2
        LEFT CHILD  (index*2) + 1
        RIGHT CHILD (index*2) + 2
    */


    
class Heap{

    public:
        Heap();
        ~Heap();

        int extractMin();
        void insertkey(int key);
        void heapifyUP();
        void heapifyDown();
        void print();

    private:
        int array [11];
        int size =0;
        void swap(int IndexOne , int IndexTwo);

        int getLeftchildIndex(int parentIndex)  {return  2*parentIndex + 1;}
        int getRightchildIndex(int parentIndex) {return  2*parentIndex + 2;}
        int getParentIndex(int childIndex)      {return  (childIndex - 1)/2;}

        /* VERY IMPORTANT  SIZE IN/DECREASE SO INDEXES CHANGE SO SIZE CANNOT BE A STATIC NUMBER*/
        bool hasLeftchild  (int Index) {return getLeftchildIndex(Index)  < size;}
        bool hasRightchild (int Index) {return getRightchildIndex(Index) < size;}
        bool hasParent     (int Index) {return getParentIndex(Index) >=0;}

        int getLeftchild  (int Index)  {return array[ getLeftchildIndex(Index) ];   }
        int getRightchild (int Index)  {return array[ getRightchildIndex(Index) ]; }
        int getParent     (int Index)  {return array[ getParentIndex(Index) ];     }
};

Heap::Heap(){
    size = 0;
}

Heap::~Heap(){}

void Heap::print(){
    int count =0;
    while (count < size){
        std::cout << array[count]<<" ";
        count++;
    }
    std::cout << '\n';
}


int Heap::extractMin(){

    if(size > 0){ // if the heap is not empty extractMin()

        int temp = array[0];
        array[0] = array[size-1];
        size--;

        heapifyDown();

        return temp;

    }else{
        std::cout << "heap empty" << '\n';
    }
}

void Heap::insertkey(int key){

    if(size < 11){ // if max capacity is not reach insert new key

        array[size] = key;
        size++;


    }


    if(size > 1){ // if max capacity is not reach insert new key

        heapifyUP();

    }

}

void Heap::swap(int IndexOne , int IndexTwo){
    int temp = array[IndexOne];
    array[IndexOne]= array[IndexTwo];
    array[IndexTwo] = temp;
}

void Heap::heapifyUP(){

    int index = size -1; //takes the last childIndex

    while( hasParent(index) && getParent(index) > array[index] ){ //if parent exist and parent is larger swap them
        swap( getParentIndex(index), index ); // swap the value of the parent and child
        index = getParentIndex(index); //update the index to the parent location which was just swap
    }

}


void Heap::heapifyDown(){

    int parentIndex = 0;

    while(hasLeftchild(parentIndex) == true){ // we only have to check left because if there is NO LEFT THEN NO RIGHT EXIST

        int smallerchildIndex = getLeftchildIndex(parentIndex);  //make the left the default samllest

        if(hasRightchild(parentIndex)== true && getRightchild(parentIndex) < getLeftchild(parentIndex)  ){ // check to see if the right is bigger

            smallerchildIndex = getRightchildIndex(parentIndex); //if the right is samller then  update samllerchildIndex

        }
        if(array[smallerchildIndex] > array[parentIndex]){
            //if the value of the samllerchildIndex is bigger the parent is smaller
            // done order is reached

            break;
        }
        else{
            //swap the value at the smallerchildIndex and parentIndex
            swap(smallerchildIndex, parentIndex);
        }

        parentIndex = smallerchildIndex;// update the parentIndex
    }
}

int main(){

    Heap heap;


    heap.insertkey(2);
    heap.insertkey(16);
    heap.insertkey(74);
    heap.insertkey(58);
    heap.insertkey(36);
    heap.insertkey(4);
    heap.insertkey(28);
    heap.insertkey(15);
    heap.insertkey(35);
    heap.insertkey(82);
    heap.insertkey(6);
        heap.print();

    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;
    cout<< "------>"<< heap.extractMin() <<endl;



    return 0;
}
