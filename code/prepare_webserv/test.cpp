#include <fstream>
#include <iostream>
#include <unistd.h>
#include "Colors.hpp"

int main(void)
{
	std::fstream	file;
	std::string		fileName("./images/animal01");

	file.open(fileName, std::ios::binary);
	std::cout << Colors::BlueString("try open " + fileName) << std::endl;
	if (file.is_open() == false)
	{
		// 4XX error
		std::cerr << Colors::RedString("open failed: " + fileName) << std::endl; 
		write(1, "HTTP/1.1 404 Not found\r\n\r\n", 26);
	}
	else
	{

	}
}
