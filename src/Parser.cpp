//
// Created by dbori on 24.06.2022.
//

#include "Parser.h"

#include <boost/asio.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/io/time_point_io.hpp>
#include <iostream>
#include <openssl/md5.h>

using namespace boost::chrono;

Parser::~Parser() { _th.join(); }

Parser::Parser(Buffer &buffer) {
  _th = std::thread(run, std::ref(buffer), std::ref(_running));
}

void Parser::run(Buffer &buffer, bool &running) {
  tMessage msg;
  while (running) {
    boost::asio::io_context io;
    boost::asio::steady_timer t(
        io, boost::asio::chrono::microseconds(INTER_PACKET_PAUSE_USEC));
    msg = buffer.get();
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)msg.data, msg.header.msg_len * sizeof(BASE_DATA_TYPE),
        md5);
    for (auto i = 0; i < msg.header.msg_len; ++i)
      msg.data[i] = ntohs(msg.data[i]);
    t.wait();
    if (strncmp(reinterpret_cast<const char *>(md5),
                reinterpret_cast<const char *>(msg.header.md5),
                MD5_DIGEST_LENGTH) != 0)
      std::cout << "Processed:\t" << msg.header.id << " "
                << boost::chrono::time_fmt(boost::chrono::timezone::local)
                << system_clock::now() << " FAIL" << std::endl;
    else
      std::cout << "Processed:\t" << msg.header.id << " "
                << boost::chrono::time_fmt(boost::chrono::timezone::local)
                << system_clock::now() << " PASS" << std::endl;
  }
}
