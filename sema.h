#ifndef SEMA_H
#define SEMA_H

#include <mutex>
#include <queue>
#include <Window.h>


class Semaphore {
  public:
    Semaphore(std::string resName);
    void down(int threadID);
    void up();
    void dump(Window* targetWin,int level);

  private:
    std::string resName;
    int sema_value;
    std::queue<pthread_t> processQueue;
    std::mutex resMutex;
    int lastPop;
};

#endif
