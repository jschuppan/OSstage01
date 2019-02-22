#include "linkedlist.h"

template<class T>
linkedlist :: linkedlist()
{
  head = NULL;
}

template<class T>
void linkedlist :: addToEnd()
{
    node* temp = head;
    if(!temp)
    {
      temp = new node;
      temp->next = NULL;
    }
    else if
    {
      while (temp->next)
      {
        temp = temp->next;
      }
      temp = temp->next;
      temp = new node;
      temp->next = NULL;
    }
}

template<class T>
void linkedlist :: addToFront()
{
    node* temp = new node;
    temp->next = head;
    head = temp;
}

template<class T>
bool linkedlist :: isEmpty()
{
  if(head)
    return false;
  else
    return true;
}

template<class T>
void linkedlist :: deleteList()
{
  while(! isEmpty())
  {
    removeFromFront();
  }
  head = NULL;
}

template<class T>
void linkedlist :: removeFromFront()
{
  node* temp = head;
  head = head->next;
  delete temp;
  temp = NULL;
}

template<class T>
void linkedlist :: removeFromEnd()
{
  node* temp = head;
  while(temp->next)
  {
    temp = temp->next;
  }
  temp->next = new node;
  temp = temp->next;
  temp->next = NULL;
}
