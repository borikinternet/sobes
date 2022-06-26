//
// Created by dbori on 24.06.2022.
//

#ifndef ITEKO_SOBES_PARSER_H
#define ITEKO_SOBES_PARSER_H

#include "Buffer.h"
#include <boost/asio/io_context.hpp>
#include <thread>

#define INTER_PACKET_PAUSE_USEC 15000

class Parser {
  std::thread _th;
  bool _running{true};
  static void run(Buffer &buffer, bool &running);
  void stop() { _running = false; }

public:
  Parser() = delete;
  ~Parser();
  explicit Parser(Buffer &buffer);
};

#endif // ITEKO_SOBES_PARSER_H
