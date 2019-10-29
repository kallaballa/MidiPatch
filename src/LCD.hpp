#ifndef SRC_LCD_HPP_
#define SRC_LCD_HPP_

#include <fstream>
#include <string>
#include <cstring>

class LCD {
	std::ofstream ttyOut_;
	char buffer0_[20];
	char buffer1_[20];
	char buffer2_[20];
	char buffer3_[20];
	char* buffers_[4] = {buffer0_, buffer1_, buffer2_, buffer3_};
public:
	LCD(const char* ttyPath) : ttyOut_(ttyPath) {
	}

	void print(uint8_t row, uint8_t col, const std::string& s);
};

#endif /* SRC_LCD_HPP_ */
