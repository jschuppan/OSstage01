#include "mem_mgr.h"

mem_mgr::mem_mgr(int size, char default_init_val) {
  
}


int mem_mgr::mam_alloc(int size) {

}


int mem_mgr::mem_free(int handle) {

}


int mem_mgr::mem_read(int handle, char *c) {

}


int mem_mgr::mem_read(int handle, int offset, int text_size, char *text) {

}


int mem_mgr::mem_write(int handle, char c) {

}


int mem_mgr::mem_write(int handle, int offset, int text_size, char *text) {

}


void mem_mgr::set_mcb(MCB *mcb) {
  this->mcb = mcb;
}


int mem_mgr::mem_left() {

}


int mem_mgr::mem_largest() {

}


int mem_mgr::mem_smallest() {

}


int mem_mgr::mem_coalesce() {

}


int mem_mgr::mem_dump() {

}
