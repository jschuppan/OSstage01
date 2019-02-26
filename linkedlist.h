/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : linkedlist.h
Date          : Febuary 25, 2019
Purpose       : Simple implementation of a linked list
============================================================================*/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <class T>
class linkedList
{
    private:
    /******************************* START PRIVATE MEMBERS ********************/
    struct node
    {
        node* next;
        T datum;
        int id;
    };

    node* head;
    int size;
    /******************************* END PRIVATE MEMBERS ********************/

    public:
    /******************************* START PUBLIC MEMBERS *******************/
    linkedList();

    void addToEnd(T datum, int id);
    void addToFront(T datum, int id);
    bool isEmpty();
    void deleteList();
    void removeFromFront();
    void removeFromEnd();
    T* getDatumById(int id);
    void removeNodeByElement(int element);
    T* getNextElement(T* ndv);
    T* getNextElementUntilEnd(T* ndv);


    int getSize();
    void setSize(int size);
    /******************************** END PUBLIC MEMBERS **********************/
};
#endif
