

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
    
    void addToEnd();
    void addToFrnt();
    bool isEmpty();
    void deleteList();
    void removeFromFront();
    void removeFromBack();
    
};
