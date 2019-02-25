#ifndef EZQUEUE_H
#define EZQUEUE_H
///// MAKE SURE TO TEMPLATE
#include <string>

template <typename T>
class ezQueue {
private:
  // define node as simple struct
  struct queueElement {
    queueElement* next;
    T content;
  };

  queueElement* head;

public:
  ezQueue();
  void enQueue(T inContent);
  T deQueue();
  bool isEmpty();
  T* getNextElement(T* initElement);
};

#endif
