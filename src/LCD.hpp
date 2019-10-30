#ifndef SRC_LCD_HPP_
#define SRC_LCD_HPP_

#include <fstream>
#include <string>
#include <cstring>

class LCD {
	std::ofstream ttyOut_;
public:
	LCD(const char* ttyPath) : ttyOut_(ttyPath) {
		clear();
	}

	LCD& print(uint8_t row, uint8_t col, const std::string& s);
	LCD& clear();

};

#endif /* SRC_LCD_HPP_ */
