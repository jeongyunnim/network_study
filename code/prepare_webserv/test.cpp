#include <fstream>
#include <iostream>
#include <unistd.h>
#include "./Colors.hpp"

int main(void)
{
	std::ifstream	file;
	std::string		fileName("./images/animal01.png");
	char			buffer[1024];

	// file.open(fileName, std::ios::in | std::ios::binary);
	file.open(fileName, std::ios::binary);
	// file.open(fileName);
	std::cout << Colors::BlueString("try open " + fileName) << std::endl;
	if (file.is_open() == false)
	{
		std::cerr << Colors::RedString("open failed: " + fileName) << std::endl; 
		write(1, "404 Not found\r\n\r\n", 16);
	}
	else
	{
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
		std::cout << "Content-length: " << fileSize << "\r\n\r\n";
		while (file.eof() == false)
		{
			file.read(buffer, sizeof(buffer));
			write(1, buffer, file.gcount());
		}
		file.close();
	}
}
