/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : ezQueue.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of ezQueue.h
============================================================================*/

//=================================
// necessary includes
#include <string>
#include "ezQueue.h"
#include "IPC.h"

// Default Constructor
template <typename T>
ezQueue<T>::ezQueue()
{
  // queue has no elements at time of creation
  head = nullptr;
  size = 0;
}

/*-----------------------------------------------------------------
Function      : enQueue(T inContext);
Parameters    : template T
Returns       : void
Details       : adds node to back of queue
------------------------------------------------------------------*/
template <typename T>
void ezQueue<T>::enQueue(T inContent) {
  // queue is empty
  if(!head)
  {
    head = new queueElement();
    head->content = inContent;
    size++;
  }
  else
  {
    // set temporary pointer to preserve
    // the head
    queueElement* temp = head;

    // loops until temp = last elemnt
    while (temp->next)
    {
      temp = temp->next;
    }

    // once at end add new element
    temp->next = new queueElement();
    temp->next->content = inContent;
    size++;
  }
}

/*-----------------------------------------------------------------
Function      : deQueue();
Parameters    :
Returns       : template T
Details       : removes first element in the queue and returns it
------------------------------------------------------------------*/
template <typename T>
T ezQueue<T>::deQueue()
{
  T contentCopy;
  queueElement* temp = head;

  // check if QUEUE is empty
  if (head != nullptr)
  {
    // make a copy of our content
    contentCopy = head->content;
    // move head pointer to next element
    head = head->next;

    // remove old node and return content
    delete temp;
    temp = nullptr;
    size--;
    return contentCopy;
  }

  // in case queue is empty say so
  else
    return contentCopy;
}

/*-----------------------------------------------------------------
Function      : isEmpty;
Parameters    :
Returns       : if head is NULL, true
                else true
Details       : checks if the queue is empty
------------------------------------------------------------------*/
template <typename T>
bool ezQueue<T>::isEmpty()
{
  //list is empty
  if (head == nullptr)
    return true;
  //List is not empty
  else
    return false;
}

/*-----------------------------------------------------------------
Function      : getNextElement(T datum);
Parameters    : template T*
Returns       : NULL if next element is last elements
                template T* next element
Details       : returns the next element in the Queue
------------------------------------------------------------------*/
template <typename T>
T* ezQueue<T>::getNextElement(T* initElement)
{
  //List is empty
  if(!head)
    return NULL;
  //parameter is empty
  if(initElement == NULL)
    return &head->content;

  //first elemnet is correct element
  if(head->content == *initElement)
    return &head->next->content;
  queueElement* temp = head;

  //loop until list = last element
  while(temp->next)
  {
    if (temp->next->content == *initElement)
    {
      if(!temp->next->next)
          return NULL;
      else
          return &temp->next->next->content;
    }
    temp = temp->next;
  }
}

template class ezQueue<std::string>;
template class ezQueue<int>;
template class ezQueue<IPC::Message_Type>;
