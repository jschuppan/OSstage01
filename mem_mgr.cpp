#include "mem_mgr.h"
#include <iostream>

using namespace std;

Mem_Mgr::Mem_Mgr(unsigned int size, unsigned char default_init_val) {
  default_mem_fill = default_init_val;
  freed_mem_fill = '#';
  capacity = available = size;
  used = 0;
  next_handle = 0;

  mem_seg ms;
  ms.handle = next_handle++;
  ms.owner_tid = -1;
  ms.start = 0;
  ms.end = capacity - 1;
  ms.size = capacity;
  ms.read_curser = 0;
  ms.write_curser = 0;
  ms.free = true;

  segments.addToFront(ms, ms.handle);

  memory = new unsigned char[capacity];
  for (unsigned int i = 0; i < capacity; i++)
    memory[i] = default_mem_fill;
}


Mem_Mgr::~Mem_Mgr() {
  delete memory;
  segments.deleteList();
}


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_alloc(unsigned int size, int tid) {
  if (size > available) {
    cout << "\nmem_alloc() : not enough memory\n";
    return -1;  //error: not enough memory
  }

  //look for a free node
  //code here

  mem_seg ms;
  ms.handle = next_handle++;
  ms.owner_tid = tid;
  //ms.start = ;
  //ms.end = ;
  //ms.size = ;
  ms.write_curser = ms.read_curser = ms.start;
  ms.free = false;

  //add new node
  //code here

  available -= size;
  used += size;

  return ms.handle;
}


int Mem_Mgr::mem_free(int handle, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_free() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    cout << "\nmem_free() : access denied\n";
    return -1;  //error: access denied
  }

  unsigned int start = ms_ptr->start;
  unsigned int end = ms_ptr->end;
  for (unsigned int i = start; i <= end; i++)
    memory[i] = freed_mem_fill;

  ms_ptr->write_curser = ms_ptr->read_curser = start;

  ms_ptr->free = true;
  ms_ptr->owner_tid = -1;

  available += ms_ptr->size;
  used -= ms_ptr->size;

  mem_coalesce();

  return 1;  //success
}


int Mem_Mgr::mem_read(int handle, unsigned char *c, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    cout << "\nmem_read() : access denied\n";
    return -1;  //error: access denied
  }

  *c = memory[ ms_ptr->read_curser ];

  ms_ptr->read_curser ++;

  //if read_curser goes past write_curser, wrap around to start
  if (ms_ptr->read_curser >= ms_ptr->write_curser)
    ms_ptr->read_curser = ms_ptr->start;

  return 1;  //success
}


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_read(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid || offset > ms_ptr->end) {
    cout << "\nmem_read() : access denied\n";
    return -1;  //error: access denied
  }

  //code here

  return 1;  //success
}


int Mem_Mgr::mem_write(int handle, unsigned char c, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    cout << "\nmem_write() : access denied\n";
    return -1;  //error: access denied
  }

  if (ms_ptr->write_curser > ms_ptr->end) {
    cout << "\nmem_write() : segment is full\n";
    return -1;  //error: segment is full
  }

  memory[ ms_ptr->write_curser ] = c;

  ms_ptr->write_curser ++;

  return 1;  //success
}


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_write(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    cout << "\nmem_write() : access denied\n";
    return -1;  //error: access denied
  }

  if (ms_ptr->write_curser > ms_ptr->end) {
    cout << "\nmem_write() : segment is full\n";
    return -1;  //error: segment is full
  }

  //code here

  return 1;  //success
}


void Mem_Mgr::set_mcb(MCB *mcb) {
  this->mcb = mcb;
}


int Mem_Mgr::mem_left() {
  return available;
}


int Mem_Mgr::mem_largest() {
  unsigned int largest = 0;

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr) {
    if (ms_ptr->free && ms_ptr->size > largest)
      largest = ms_ptr->size;
    
    ms_ptr = getNextElementUntilEnd(ms_ptr);
  }

  return largest;
}


int Mem_Mgr::mem_smallest() {
  unsigned int smallest = capacity;

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr) {
    if (ms_ptr->free && ms_ptr->size < smallest)
      smallest = ms_ptr->size;
    
    ms_ptr = getNextElementUntilEnd(ms_ptr);
  }

  return smallest;
}


int Mem_Mgr::mem_coalesce() {
  mem_seg *prev_ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  mem_seg *ms_ptr = getNextElementUntilEnd(prev_ms_ptr);
  while (ms_ptr) {
    if (prev_ms_ptr->free && ms_ptr->free) {  //adjacent free segments

      //fill prev_mem_ptr with '.'
      if (memory[ prev_ms_ptr->start ] != default_mem_fill) {
	int start = prev_ms_ptr->start;
	int end = prev_ms_ptr->end;
	for (int i = start; i <= end; i++)
	  memory[i] = default_mem_fill;
      }

      //fill mem_ptr with '.'
      if (memory[ ms_ptr->start ] != default_mem_fill) {
	int start = ms_ptr->start;
	int end = ms_ptr->end;
	for (int i = start; i <= end; i++)
	  memory[i] = default_mem_fill;
      }

      //combine
      ms_ptr->start = prev_ms_ptr->start;
      ms_ptr->size += prev_ms_ptr->size;
      removeNodeByElement(prev_ms_ptr->handle);
      ms_ptr->write_curser = ms_ptr->read_curser = ms_ptr->start;
    }
    
    prev_ms_ptr = ms_ptr;
    ms_ptr = getNextElementUntilEnd(ms_ptr);
  }

  return 1;  //success
}


int Mem_Mgr::mem_dump() {

}
