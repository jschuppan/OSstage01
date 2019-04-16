/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : linkedlist.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of linkedlist.h
============================================================================*/

//=================================
// Forward declarations
class Window;

//=================================
// necessary includes
#include <ncurses.h>
#include "scheduler.h"
#include "linkedlist.h"
#include "IPC.h"
#include "mem_mgr.h"


//defualt constructor
template<class T>
linkedList<T> :: linkedList()
{
  head = NULL;
  size = 0;
}

/*-----------------------------------------------------------------
Function      : addToEnd(T datum, int id);
Parameters    : template T , int
Returns       : void
Details       : adds node to end of list and populates data
------------------------------------------------------------------*/
template<class T>
void linkedList<T> :: addToEnd(T datum, int id)
{
    node* temp = head;
    //List is empty
    if(!head)
    {
      head = new node;
      head->next = NULL;
      head->datum = datum;
      head->id = id;
      size++;
    }
    //List is not empty
    else
    {
      //loop until temp = last node
      while (temp->next)
      {
        temp = temp->next;
      }
      //delete last node
      temp->next = new node;
      temp = temp->next;
      temp->datum = datum;
      temp->id = id;
      temp->next = NULL;
      size++;
    }
}

/*-----------------------------------------------------------------
Function      : addToFront(T datum, int id);
Parameters    : template T , int
Returns       : void
Details       : adds node to front of list and populates data
------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------
Function      : isEmpty();
Parameters    :
Returns       : if head id not NULL returns true
                else returns false
Details       : checks if the list is empty
------------------------------------------------------------------*/
template<class T>
bool linkedList<T> :: isEmpty()
{
  //List is not empty
  if(head)
    return false;
  //list is empty
  else
    return true;
}

/*-----------------------------------------------------------------
Function      : deleteList();
Parameters    :
Returns       : void
Details       : deletes all elements in the list
------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------
Function      : removeFromFront();
Parameters    :
Returns       : void
Details       : removes element at the fron of list
------------------------------------------------------------------*/
template<class T>
void linkedList<T> :: removeFromFront()
{
  //delete first node
  node* temp = head;
  head = head->next;
  delete temp;
  temp = NULL;
  size--;
}

/*-----------------------------------------------------------------
Function      : removeFromEnd();
Parameters    :
Returns       : deletes last node in the list
Details       : void
------------------------------------------------------------------*/
template<class T>
void linkedList<T> :: removeFromEnd()
{
  node* temp = head;
  //loop until temp = last node
  while(temp->next)
  {
    temp = temp->next;
  }
  //delete temp
  temp->next = new node;
  temp = temp->next;
  temp->next = NULL;
  size--;
}

/*-----------------------------------------------------------------
Function      : getDatumById(int element);
Parameters    : int elementIndex
Returns       : if element does not exist return NULL
                else template T*
Details       : returns datum of the given elements node
------------------------------------------------------------------*/
template<class T>
T* linkedList<T> :: getDatumById(int id)
{
  node* temp = head;
  //list is empty
  if(!temp)
    return NULL;
  //loops until id is found
  while(temp->id != id)
  {
    temp = temp->next;
    //if no more elements return NULL
    if(temp == NULL)
      return NULL;
  }
  return &temp->datum;
}

/*-----------------------------------------------------------------
Function      : removeNodeByElement(int element);
Parameters    : int elementIndex
Returns       : void
Details       : deletes node at corresponing element int List
------------------------------------------------------------------*/
template<class T>
void linkedList<T> :: removeNodeByElement(int element)
{
    //empty list
  if(!head)
    return;
  node* temp = head;
  //If first element needs deleted
  if(temp->id == element)
  {
      head = head->next;
    	delete temp;
    	temp = NULL;
    	size--;
    	return;
  }
  node* temp1 = NULL;
  char buff[255];
  //Loop until temp = last node
  while(temp->next)
  {
      //next element needs deleted
      if(temp->next->id == element)
      {
        temp1 = temp->next;
        temp->next = temp->next->next;
        delete temp1;
	      size--;
        return;
      }
      temp = temp->next;
  }
}

/*-----------------------------------------------------------------
Function      : getNextElement(T* ndv);
Parameters    : template T* type
Returns       : if ndv is NULL, returns from front node datum;
                if ndv is last elemnt in list, returns first element
                else returns next node datum
Details       : returns next node datum, simulates circular linked list
------------------------------------------------------------------*/
template<class T>
T* linkedList<T> ::  getNextElement(T* ndv)
{
  //List is empty
  if(!head)
    return NULL;
  //parameter is NULL
  if(ndv == NULL)
    return &head->datum;

  node* temp = head;
  //one element in list
  if(head->datum == *ndv)
    return &head->next->datum;
  //loop until temp = last element
  while(temp->next)
  {
    //if next element datum = parameter return next datum
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

/*-----------------------------------------------------------------
Function      : getNextElementUntilEnd(T* ndv);
Parameters    : template T* type
Returns       : if ndv is NULL, returns from front node datum;
                if ndv is last elemnt in list, returns NULL
                else returns next node datum
Details       : returns next node datum
------------------------------------------------------------------*/
template<class T>
T* linkedList<T> ::  getNextElementUntilEnd(T* ndv)
{
  //List is empty
  if(!head)
    return NULL;
  //parameter is NULL
  if(ndv == NULL)
    return &head->datum;

  node* temp = head;
  //one element in List
  if(head->datum == *ndv)
    return &head->next->datum;

  //loop until temp = last node
  while(temp->next)
  {
    //if next datam = parameter return next datum
    if (temp->next->datum == *ndv)
    {
      if(!temp->next->next)
          return NULL;
      else
          return &temp->next->next->datum;
    }
    temp = temp->next;
  }
}

/*-----------------------------------------------------------------
Function      : insertNode
Parameters    : template T* type, integer ID, integer position
Returns       :
Details       : inserts a node at given position if position exists
------------------------------------------------------------------*/
template<class T>
void linkedList<T> :: insertNode(T datum, int id, int position)
{
  node* temp = head;
  //position does not exist
  if(size < position || position < 0)
  {
    return;
  }
  //Position exists
  else if (position == 0)
  {
    head = new node;
    head -> next = temp;
    head->datum = datum;
    head->id = id;
    size++;
  }
  else
  {
    int count = 1;
    //loop until position found
    while (count != position)
    {
      temp = temp->next;
      count++;
    }
    //delete last node
    node* tempNext = temp->next;
    temp->next = new node;
    temp = temp->next;
    temp->datum = datum;
    temp->id = id;
    temp->next = tempNext;
    size++;
    //cout<<"inserted";
  }
}

template<class T>
void linkedList<T> :: swapNodes(int leftId)
{
  node* L = head;
  node* R = NULL;

  //list is empty
  if(!head)
    return;
    //first node is to be swaped
  if(head->id == leftId)
  {
    R = head->next;
    head = R;
    L->next = R->next;
    R->next = L;

    return;
  }

  //loop untill left Id is found
  node* temp = head;
  while(temp->next)
  {
    if(leftId == temp->next->id)
    {
      L = temp->next;
      break;
    }
    temp = temp->next;
  }

  R = L->next;
  L->next = R->next;
  R->next = L;
  temp->next = R;


}
//Accessors
template<class T>
int linkedList<T> :: getSize(){return size;}


//Mutators
template<class T>
void linkedList<T> :: setSize(int size){size = size;}

template class linkedList<int>;
template class linkedList<pthread_t>;
template class linkedList<Scheduler::thread_data>;
template class linkedList<Window>;
template class linkedList<Scheduler::TCB>;
template class linkedList<ezQueue<IPC::Message_Type> >;
template class linkedList<Mem_Mgr::mem_seg>;
