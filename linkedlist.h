#ifndef LINKEDLIST_H
#ifndef LINKEDLIST_H

template <class T>
class linkedList
{
    private:
    struct node
    {
        node* next;
        T datum;
    }

    node* head;

    public:
    linkedList();

    void addToEnd(T datum);
    void addToFront(T datum);
    bool isEmpty();
    void deleteList();
    void removeFromFront();
    void removeFromEnd();

};

#endif
