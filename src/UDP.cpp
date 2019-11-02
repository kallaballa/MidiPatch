#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <limits>

#include "UDP.hpp"

namespace farts {

UDP::UDP(size_t port) {
  // Creating socket file descriptor
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
      perror("socket creation failed");
      exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = INADDR_ANY;
}

UDP::~UDP() {
	// TODO Auto-generated destructor stub
}

void UDP::send(const std::vector<int16_t>& samples) {
  size_t lenBuf = samples.size();
  sendto(sockfd, (const char *) samples.data(), lenBuf * sizeof(int16_t),
	MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
}
} /* namespace farts */
