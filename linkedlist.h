#pragma once

#include "window.h"
// #include "scheduler.h"

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
