#ifndef SEMA_H
#define SEMA_H

class Semaphore {
  public:
    Semaphore(std::string resName);
    void printName();
    void down(int threadID);
    void up();
    bool isAvailable();

  private:
    std::string resName;
    int sema_value;
    std::queue<int> processQueue;
};

#endif
