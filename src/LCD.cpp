/*
 * LCD.cpp
 *
 *  Created on: Oct 29, 2019
 *      Author: elchaschab
 */

#include "LCD.hpp"

LCD& LCD::print(uint8_t row, uint8_t col, const std::string& s) {
	size_t len = std::min((size_t)20, s.size() + col);
	memcpy((void *)(buffers_[row] + col), (void *)s.c_str(), len);
	ttyOut_ << row;
	ttyOut_.write(buffers_[row], 20);
	ttyOut_.flush();
	return *this;
}

LCD& LCD::clear() {
	for(uint8_t i = 0; i < 4; ++i) {
		memset((void *)(buffers_[i]), 0, 20);
		ttyOut_ << i;
		ttyOut_.write(buffers_[i], 20);
		ttyOut_.flush();
	}
	return *this;
}
