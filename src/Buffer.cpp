//
// Created by dbori on 24.06.2022.
//

#include "Buffer.h"

#include <boost/atomic.hpp>
#include <semaphore.h>

Buffer::Buffer(const unsigned char len) : _len(len) {
  sem_init(&_read_sem, false, 0);
  sem_init(&_write_sem, false, len);
  for (int i = 0; i < len; ++i)
    _data.emplace_back();
}

bool Buffer::put(tMessage &message) {
  if (sem_trywait(&_write_sem) == -1)
    return false;
  _data[++_head] = message;
  _head %= _len;
  sem_post(&_read_sem);
  return true;
}

tMessage Buffer::get() {
  sem_wait(&_read_sem);
  auto res = _data[++_tail];
  _tail %= _len;
  sem_post(&_write_sem);
  return res;
}
