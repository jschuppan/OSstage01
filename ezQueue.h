/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : ezQueue.h
Date          : Febuary 25, 2019
Purpose       : Simple implementation of a Queue
============================================================================*/
#ifndef EZQUEUE_H
#define EZQUEUE_H


template <typename T>
class ezQueue {
private:
  /******************************* START PRIVATE MEMBERS ********************/
  struct queueElement {
    queueElement* next;
    T content;
  };

  queueElement* head;
  int size;
  /******************************* END PRIVATE MEMBERS ********************/

public:
  /******************************* START PUBLIC MEMBERS *******************/
  ezQueue();
  void enQueue(T inContent);
  T deQueue();
  bool isEmpty();
  int getSize() {return this->size;}
  T* getNextElement(T* initElement);

  bool operator==(const ezQueue& rhs)
  {
    return (head == rhs.head);
  }
  /******************************** END PUBLIC MEMBERS **********************/

};

#endif
