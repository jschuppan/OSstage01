#ifndef MEM_MGR_H
#define MEM_MGR_H

#include "linkedlist.h"

class MCB;

//memory segment
struct mem_seg {
  int handle;
  int owner_tid;
  unsigned int start, end, size;
  unsigned int read_cursor, write_cursor;  //indeces for mem_read() and mem_write()
  bool free;
};


//memory manager
class Mem_Mgr {

  /****************************  Public start  ********************************/
 public:

  Mem_Mgr(unsigned int size = 1024, unsigned char default_init_val = '.');
  ~Mem_Mgr();
  int mem_alloc(unsigned int size,  int tid);
  int mem_free(int handle, int tid);
  int mem_read(int handle, unsigned char *c, int tid);
  int mem_read(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid);
  int mem_write(int handle, unsigned char c, int tid);
  int mem_write(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid);
  void setMCB(MCB *mcb);

  /*****************************  Public end  *********************************/

  /***************************  Private start  ********************************/
 private:

  MCB *mcb;
  unsigned char *memory;
  unsigned char default_mem_fill;  // '.'
  unsigned char freed_mem_fill;  // '#'
  unsigned int capacity, available, used;
  int next_handle;
  linkedList<mem_seg> segments;

  int mem_left();
  int mem_largest();
  int mem_smallest();
  int mem_coalesce();
  void mem_dump();
  /****************************  Private end   ********************************/

};

#endif
