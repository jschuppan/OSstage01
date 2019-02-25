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
    struct node
    {
        node* next;
        T datum;
        int id;
        friend linkedList;
    };

    node* head;
    int size;

    public:
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
};
#endif
