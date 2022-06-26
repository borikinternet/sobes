//
// Created by dbori on 23.06.2022.
//

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "sobes.h"

#define USAGE                                                                  \
  "Test client, should be started after server\n"                              \
  "Usage: %s <server address> <server port>\n"                                 \
  "Only IPv4 allowed"

#define MESSAGE_CYCLE_COUNT 1000
#define SESSION_COUNT 2
#define INTER_PACKET_PAUSE_USEC 10000
#define INTER_SESSION_PAUSE_USEC 10000000

int get_connected_socket(char *, char *);
size_t generate_packet_data(struct timeval tv, unsigned id, tMessage *buffer);

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stdout, USAGE, argv[0]);
    exit(-1);
  }

  char *server_name = argv[1];
  char *port = argv[2];

  int f_sock = get_connected_socket(server_name, port);
  if (f_sock == -1)
    exit(-1); // all error messages already printed

  int count = 0;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);

  void *buf = malloc(sizeof(tMessage));
  if (!buf) {
    fprintf(stderr, "Not enough memory!\n");
    exit(-1);
  }

  for (int i = 0; i < SESSION_COUNT; ++i) {
    for (int j = 0; j < MESSAGE_CYCLE_COUNT; ++j) {
      struct tm *nowtm = localtime(&tv.tv_sec);
      size_t msg_len = generate_packet_data(tv, count, buf);
      ssize_t sent;
      while ((sent = send(f_sock, buf, msg_len, 0)) < msg_len && sent != -1)
        msg_len -= sent;
      if (sent == -1) {
        fprintf(stderr, "Socket broken! Exiting...\n");
        exit(-1);
      }
      printf("Sent: %d at %04i-%02i-%02i %02i:%02i:%02i.%06ld\n", count,
             nowtm->tm_year+1900, nowtm->tm_mon, nowtm->tm_mday, nowtm->tm_hour,
             nowtm->tm_min, nowtm->tm_sec, tv.tv_usec);
      struct timeval _tv;
      gettimeofday(&_tv, NULL);
      usleep(INTER_PACKET_PAUSE_USEC - (_tv.tv_usec - tv.tv_usec));
      gettimeofday(&tv, NULL);
      ++count;
    }
    if (i < SESSION_COUNT - 1)
      usleep(INTER_SESSION_PAUSE_USEC - INTER_PACKET_PAUSE_USEC);
    gettimeofday(&tv, NULL);
  }

  free(buf);
  close(f_sock);
}

int get_connected_socket(char *server, char *port) {
  struct addrinfo req, *local_ep;
  memset(&req, 0, sizeof req);
  req.ai_family = AF_INET;
  req.ai_socktype = SOCK_STREAM;

  int err_code;
  if ((err_code = getaddrinfo(server, port, &req, &local_ep)) != 0) {
    fprintf(stderr, "Can't setup outgoing connection: %s\n",
            gai_strerror(err_code));
    exit(-1);
  }

  int f_socket = -1;
  struct addrinfo *candidate;
  for (candidate = local_ep; candidate && (f_socket == -1 || err_code == -1);
       candidate = candidate->ai_next) {
    if ((f_socket = socket(candidate->ai_family, candidate->ai_socktype,
                           candidate->ai_protocol)) != -1) {
      if ((err_code = connect(f_socket, candidate->ai_addr,
                              candidate->ai_addrlen)) == -1)
        close(f_socket);
      else
        break;
    }
  }

  if (f_socket == -1) {
    fprintf(stderr, "Can't open socket to connect %s\n", server);
    return -1;
  }

  if (err_code == -1 || !candidate) {
    fprintf(stderr, "Can't connect to %s\n", server);
    return -1;
  }
  freeaddrinfo(local_ep);

  return f_socket;
}

size_t generate_packet_data(struct timeval tv, unsigned id, tMessage *buffer) {
  int len = 600 + rand() % (DATA_MAX_LEN - DATA_MIN_LEN);
  buffer->header.id = htonl(id);
  buffer->header.msg_len = htons(len);
  buffer->header.send_time.tv_sec = htonl(tv.tv_sec);
  buffer->header.send_time.tv_usec = htonl(tv.tv_usec);
  for (int i = 0; i < len; ++i)
    buffer->data[i] = htons(rand()); // todo move it to external function
  MD5((void *)buffer->data, sizeof(BASE_DATA_TYPE) * len, buffer->header.md5);
  return len * sizeof(BASE_DATA_TYPE) + sizeof(tMessageHeader);
}
