#include "linkedlist.h"
#include "window.h"
#include "scheduler.h"
#include<iostream>
#include "linkedlist.h"

using namespace std;
template<class T>
linkedList<T> :: linkedList()
{
  head = NULL;
  size = 0;
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
      size++;
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
      size++;
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
    size++;
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
  size = 0;
}

template<class T>
void linkedList<T> :: removeFromFront()
{
  node* temp = head;
  head = head->next;
  delete temp;
  temp = NULL;
  size--;
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
  size--;
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

template<class T>
void linkedList<T> :: removeNodeByElement(int element)
{
    //empty list
    if(!head)
      return;
    node* temp = head;
    if(temp->id == element)
    {
      cout<<"REASSIGN HEAD"<<endl;
        head = head->next;
	cout<<"ABOUT TO DELETE TEMP"<<endl;
	delete temp;
	cout<<"TEMP DELETED" <<endl;
	temp = NULL;
	size--;
	return;
    }

    while(temp->next)
    {
      if(temp->next->id == element)
      {
        temp->next = temp->next->next;
        delete temp->next;
	       size--;
        return;
      }
      temp = temp->next;
    }
}

template<class T>
T* linkedList<T> ::  getNextElement(T* ndv)
{

  if(!head) {
  //std::cout << "its me the devil" << std::endl;
    return NULL;
  }
  if(ndv == NULL)
  {
    return &head->datum;
  }
    node* temp = head;

  if(head->datum == *ndv)
  {
    return &head->next->datum;
  }

  while(temp->next)
  {
    if (temp->next->datum == *ndv)
    {
      if(!temp->next->next)
          return &head->datum;
      else
          return &temp->next->next->datum;
    }
    temp = temp->next;
  }
 }


template<class T>
int linkedList<T> :: getSize(){return size;}

template<class T>
void linkedList<T> :: setSize(int size){size = size;}

template class linkedList<int>;
template class linkedList<Window>;
template class linkedList<Scheduler::TCB>;
