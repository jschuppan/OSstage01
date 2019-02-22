#pragma once

#include <mutex>
#include <queue>


class Semaphore {
  public:
    Semaphore(std::string resName);
    void down(int threadID);
    void up();

  private:
    std::string resName;
    int sema_value;
    std::queue<pthread_t> processQueue;
    std::mutex resMutex;
    pthread_t lastPop;
};
