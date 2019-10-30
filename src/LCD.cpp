/*
 * LCD.cpp
 *
 *  Created on: Oct 29, 2019
 *      Author: elchaschab
 */

#include "LCD.hpp"

LCD& LCD::print(uint8_t row, uint8_t col, const std::string& s) {
	ttyOut_ << row;
	for(size_t i = 0; i < col; ++i)
		ttyOut_ << ' ';
	ttyOut_.write(s.c_str(), 20 - col);
	ttyOut_.flush();
	return *this;
}

LCD& LCD::clear() {
	ttyOut_ << 4;
	ttyOut_.flush();
	return *this;
}
