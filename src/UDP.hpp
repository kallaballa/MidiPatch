/*
 * UDP.hpp
 *
 *  Created on: Nov 2, 2019
 *      Author: elchaschab
 */

#ifndef SRC_UDP_HPP_
#define SRC_UDP_HPP_

#include <vector>
#include <netinet/in.h>

namespace farts {

class UDP {
	int sockfd;
  struct sockaddr_in servaddr;
public:
	UDP(size_t port);
	virtual ~UDP();
	void send(const std::vector<int16_t>& data);
};

} /* namespace farts */

#endif /* SRC_UDP_HPP_ */
