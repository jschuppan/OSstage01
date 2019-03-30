#ifndef MEM_MGR_H
#define MEM_MGR_H

#include "linkedlist.h"

class MCB;

struct mem_segment {
  int handle;
  int owner_tid;
  int start, end, size;
  int read_curser, write_curser;  //indeces for mem_read() and mem_write()
  bool used;
};

class mem_mgr {

  /****************************  Public start  ********************************/
 public:

  mem_mgr(int size, char default_init_val);
  int mem_alloc(int size);
  int mem_free(int handle);

  int mem_read(int handle, char *c);
  int mem_read(int handle, int offset, int text_size, char *text);

  int mem_write(int handle, char c);
  int mem_write(int handle, int offset, int text_size, char *text);

  void set_mcb(MCB *mcb);

  /*****************************  Public end  *********************************/

  /***************************  Private start  ********************************/
 private:

  MCB *mcb;
  unsigned char *memory;
  int capacity, available, used;
  linkedList<mem_segment> segments;

  int mem_left();
  int mem_largest();
  int mem_smallest();
  int mem_coalesce();
  int mem_dump();
  /****************************  Private end   ********************************/

};

#endif
