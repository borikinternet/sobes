//
// Created by dbori on 23.06.2022.
//

#include <boost/asio.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "Getter.h"
#include "Parser.h"

#define USAGE                                                                  \
  "Test server, listen on <address>:<port>\n"                                  \
  "Usage: %s <port>\n"
#define BUF_LENGTH 16

using namespace boost::asio;

int get_listen_socket(char *);

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stdout, USAGE, argv[0]);
    exit(-1);
  }

  try {
    boost::asio::io_context io_context;
    ip::tcp::acceptor acceptor(
        io_context, ip::tcp::endpoint(ip::tcp::v4(), std::atoi(argv[1])));
    ip::tcp::socket socket(io_context);
    acceptor.accept(socket);

    Buffer buffer(BUF_LENGTH);
    // todo fix work with context
    Parser parser(buffer);
    Getter getter(socket, buffer);

    getter.Run();
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(-1);
  }
}