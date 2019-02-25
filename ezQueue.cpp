// ezQueue.cpp:   Basic implementation of a templated Queue
// Author:        Jakob Schuppan
// Date:          01-18-2019
// Assignment:    HW01 Part01
#include <string>
#include <iostream>
#include "ezQueue.h"

// Standard Constructor
template <typename T>
ezQueue<T>::ezQueue() {
  // queue has no elements at time of creation
  head = nullptr;
}

// enQueue:  This function enQueues an element
// input(s): T - whichever type of content will be stored
// returns:  nothing(void)
template <typename T>
void ezQueue<T>::enQueue(T inContent) {
  // check if queue is empty
  if(head == nullptr) {
    head = new queueElement();
    head->content = inContent;
  }
  else {
    // set temporary pointer to preserve
    // the head
    queueElement* temp = head;

    // traverse to the second last element
    while (temp->next != nullptr) {
      temp = temp->next;
    }

    // once at end add new element
    temp->next = new queueElement();
    temp->next->content = inContent;
  }
}

// deQueue:  This function deQueues an element
// input(s): none
// returns:  T - whichever type of content is stored
template <typename T>
T ezQueue<T>::deQueue() {
  std::string contentCopy;
  queueElement* temp = head;

  // check if QUEUE is empty
  if (head != nullptr) {

    // make a copy of our content
    contentCopy = head->content;
    // move head pointer to next element
    head = head->next;

    // remove old node and return content
    delete temp;
    temp = nullptr;
    return NULL;
  }

  // in case queue is empty say so
  else {
    return NULL;
  }
}

// isEmpty:  This function checks if the Queue is empty
// input(s): none
// returns:  boolean - true if empty, false if not
template <typename T>
bool ezQueue<T>::isEmpty() {
  if (head == nullptr) {
    return true;
  }
  else {
    return false;
  }
}

T* getNextElement(T* initElement)
{
  if(!head)
    return NULL;
  if(initElement == NULL)
    return &head->content;
  if(head->datum == *initElement)
    return &head->next->content;
  queueElement* temp = head;
  while(temp->next)
  {
    if (temp->next->datum == *initElement)
    {
      if(!temp->next->next)
          return NULL;
      else
          return &temp->next->next->datum;
    }
    temp = temp->next;
  }
}

template class ezQueue<std::string>;
template class ezQueue<int>;
