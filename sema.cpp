#include <cstdio>
#include "sema.h"


Semaphore::Semaphore(std::string resName) {
  this->resName = resName;
  this->sema_value = 1;
}

void Semaphore::down(int threadID) {
  // case 1: resource is available
  if (sema_value == 1) {

  }
  // case 2: resource is unavailable
  else if (sema_value == 0) {

  }

  else {
    p
  }
}
void Semaphore::up() {

}
