//
// Created by Dmitrii Borisov on 24.06.2022.
//

#include "Getter.h"
#include "sobes.h"

#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/io/time_point_io.hpp>
#include <iostream>
#include <openssl/md5.h>

void Getter::Run() {
  volatile auto dataHeader = static_cast<tMessage *>(malloc(sizeof(tMessage)));
  mutable_buffer buf(dataHeader, sizeof(tMessageHeader));
  while (_s.is_open()) {
    read(_s, buf);

    dataHeader->header.id = ntohl(dataHeader->header.id);
    dataHeader->header.msg_len = ntohs(dataHeader->header.msg_len);
    dataHeader->header.send_time.tv_sec =
        ntohl(dataHeader->header.send_time.tv_sec);
    dataHeader->header.send_time.tv_usec =
        ntohl(dataHeader->header.send_time.tv_usec);

    mutable_buffer data_buf(static_cast<void *>(dataHeader->data),
                            dataHeader->header.msg_len *
                                sizeof(BASE_DATA_TYPE));
    read(_s, data_buf);

    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)dataHeader->data,
        dataHeader->header.msg_len * sizeof(BASE_DATA_TYPE), md5);
    if (strncmp(reinterpret_cast<const char *>(md5),
                reinterpret_cast<const char *>(dataHeader->header.md5),
                MD5_DIGEST_LENGTH) != 0)
      std::cout << "Received:\t" << dataHeader->header.id << " "
                << boost::chrono::time_fmt(boost::chrono::timezone::local)
                << boost::chrono::system_clock::now() << " FAIL" << std::endl;
    else
      std::cout << "Received:\t" << dataHeader->header.id << " "
                << boost::chrono::time_fmt(boost::chrono::timezone::local)
                << boost::chrono::system_clock::now() << " PASS" << std::endl;
    _b.put(*dataHeader);
  }
}
