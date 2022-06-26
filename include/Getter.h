//
// Created by dbori on 24.06.2022.
//

#ifndef ITEKO_SOBES_GETTER_H
#define ITEKO_SOBES_GETTER_H

#include "Buffer.h"
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

class Getter {
  ip::tcp::socket &_s;
  Buffer &_b;
public:
  Getter() = delete;
  Getter(ip::tcp::socket &socket, Buffer &buffer) : _s{socket}, _b{buffer} {};
  void Run();
};

#endif // ITEKO_SOBES_GETTER_H
