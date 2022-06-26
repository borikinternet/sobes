//
// Created by dbori on 24.06.2022.
//

#ifndef ITEKO_SOBES_SOBES_H
#define ITEKO_SOBES_SOBES_H

#ifdef __cplusplus
#include <cstdint>
#include <ctime>
#endif
#include <openssl/md5.h>

#define DATA_MIN_LEN 600
#define DATA_MAX_LEN 1600
#define BASE_DATA_TYPE int16_t

typedef struct {
  unsigned id;
  uint16_t msg_len;
  struct timeval send_time;
  unsigned char md5[MD5_DIGEST_LENGTH];
} tMessageHeader;

typedef struct {
  tMessageHeader header;
  BASE_DATA_TYPE data[DATA_MAX_LEN];
} tMessage;

#endif // ITEKO_SOBES_SOBES_H
