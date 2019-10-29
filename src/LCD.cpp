/*
 * LCD.cpp
 *
 *  Created on: Oct 29, 2019
 *      Author: elchaschab
 */

#include "LCD.hpp"

void LCD::print(uint8_t row, uint8_t col, const std::string& s) {
	size_t len = std::min((size_t)20, s.size() - col);
	memcpy((void *)(buffers_[row] + col), (void *)s.c_str(), len);
	ttyOut_ << row;
	ttyOut_.write(buffers_[row], 20);
	ttyOut_.flush();
}

