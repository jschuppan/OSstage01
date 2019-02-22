#include "linkedlist.h"
#include "window.h"
#include "scheduler.h"
#include<iostream>

template<class T>
linkedList<T> :: linkedList()
{
  head = NULL;
}

template<class T>
void linkedList<T> :: addToEnd(T datum, int id)
{
    node* temp = head;
    if(!temp)
    {
      temp = new node;
      temp->next = NULL;
      temp->datum = datum;
      temp->id = id;
    }
    else
    {
      while (temp->next)
      {
        temp = temp->next;
      }
      temp->next = new node;
      temp = temp->next;
      temp->datum = datum;
      temp->id = id;
      temp->next = NULL;
    }
}

template<class T>
void linkedList<T> :: addToFront(T datum, int id)
{
    node* temp = new node;
    temp->next = head;
    head = temp;
    head->datum = datum;
    head->id = id;
}

template<class T>
bool linkedList<T> :: isEmpty()
{
  if(head)
    return false;
  else
    return true;
}

template<class T>
void linkedList<T> :: deleteList()
{
  while(! isEmpty())
  {
    removeFromFront();
  }
  head = NULL;
}

template<class T>
void linkedList<T> :: removeFromFront()
{
  node* temp = head;
  head = head->next;
  delete temp;
  temp = NULL;
}

template<class T>
void linkedList<T> :: removeFromEnd()
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

template<class T>
T* linkedList<T> :: getDatumById(int id)
{
  node* temp = head;
  if(!temp)
    return NULL;
  while(temp->id != id)
  {
    temp = temp->next;
    if(temp == NULL)
      return NULL;
  }
  return &temp->datum;
}


template class linkedList<int>;
template class linkedList<Window>;
template class linkedList<Scheduler::TCB>;
