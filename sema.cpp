#include "sema.h"

Semaphore::Semaphore(std::string resName) {
  this->resName = resName;
  this->sema_value = 1;
}

void Semaphore::down(int threadID);
void Semaphore::up();
