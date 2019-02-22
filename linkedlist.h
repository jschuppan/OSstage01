#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "window.h"

template <class T>
class linkedList
{
    private:
    struct node
    {
        node* next;
        T datum;
        int id;
    };

    node* head;

    public:
    linkedList();

    void addToEnd(T datum, int id);
    void addToFront(T datum, int id);
    bool isEmpty();
    void deleteList();
    void removeFromFront();
    void removeFromEnd();
    T* getDatumById(int id);
};

template class linkedList<int>;
template class linkedList<Window>;
#endif
