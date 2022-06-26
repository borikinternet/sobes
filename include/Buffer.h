//
// Created by dbori on 24.06.2022.
//

#ifndef ITEKO_SOBES_BUFFER_H
#define ITEKO_SOBES_BUFFER_H

#include "sobes.h"
#include <semaphore.h>
#include <vector>

#define MAX_BUF_LENGTH UINT8_MAX

class Buffer {
  std::vector<tMessage> _data{MAX_BUF_LENGTH};
  volatile unsigned char _head{}, _tail{};
  const unsigned char _len;
  sem_t _read_sem{}, _write_sem{};
public:
  Buffer() = delete;
  explicit Buffer(unsigned char len);

  bool put(tMessage &message);
  tMessage get();
};

#endif // ITEKO_SOBES_BUFFER_H
