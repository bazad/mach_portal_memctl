#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "memctl/core.h"
#include "memctl/task_memory.h"

/* read */

uint32_t r32(mach_port_t tp, uint64_t addr) {
  uint32_t value;
  task_io_result tr = task_read_word(task_read, tp, addr, &value, sizeof(value), 0);
  assert(tr == TASK_IO_SUCCESS);
  return value;
}

uint64_t r64(mach_port_t tp, uint64_t addr) {
  uint64_t value;
  task_io_result tr = task_read_word(task_read, tp, addr, &value, sizeof(value), 0);
  assert(tr == TASK_IO_SUCCESS);
  return value;
}

void* rmem(mach_port_t tp, uint64_t addr, uint64_t len) {
  uint8_t* outbuf = malloc(len);
  size_t size = len;
  task_io_result tr = task_read(tp, addr, &size, outbuf, 0, NULL);
  assert(tr == TASK_IO_SUCCESS);
  return outbuf;
}

/* write */

void w8(mach_port_t tp, uint64_t addr, uint8_t val) {
  task_io_result tr = task_write_word(task_write, tp, addr, val, sizeof(val), 0);
  assert(tr == TASK_IO_SUCCESS);
}

void w32(mach_port_t tp, uint64_t addr, uint32_t val) {
  task_io_result tr = task_write_word(task_write, tp, addr, val, sizeof(val), 0);
  assert(tr == TASK_IO_SUCCESS);
}

void w64(mach_port_t tp, uint64_t addr, uint64_t val) {
  task_io_result tr = task_write_word(task_write, tp, addr, val, sizeof(val), 0);
  assert(tr == TASK_IO_SUCCESS);
}

/* wrappers with implict kernel task port argument */

// NOTE: It would be better to use the kernel_memory.h functions here, but mach_portal doesn't
// really need it.

uint64_t rk64(uint64_t addr) {
  return r64(kernel_task, addr);
}

uint32_t rk32(uint64_t addr) {
  return r32(kernel_task, addr);
}

void* rkmem(uint64_t addr, uint64_t len) {
  return rmem(kernel_task, addr, len);
}

void wk8(uint64_t addr, uint8_t val) {
  w8(kernel_task, addr, val);
}

void wk32(uint64_t addr, uint32_t val) {
  w32(kernel_task, addr, val);
}

void wk64(uint64_t addr, uint64_t val) {
  w64(kernel_task, addr, val);
}
