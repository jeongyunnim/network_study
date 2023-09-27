#include <iostream>
#include <sstream>

int main()
{
	std::stringstream ss;
	std::string 	temp;

	ss << "HELLO WORLD.\r\n세상님 안녕하세요.\r\n";

	std::getline(ss, temp, '\n');
	ss.seekg(0, std::ios::end);
	std::cout << "line: " << temp << std::endl;
	std::cout << "offset: " << ss.tellg() << std::endl;
	std::cout << "good: " << ss.good() << std::endl;
	std::cout << "bad: " << ss.bad() << std::endl;
	std::cout << "eof: " << ss.eof() << std::endl;
	std::cout << "fail: " << ss.fail() << std::endl;
	ss.seekg(0);
	std::getline(ss, temp, '\n');
	std::cout << "line: " << temp << std::endl;
	std::cout << "offset: " << ss.tellg() << std::endl;
	std::cout << "good: " << ss.good() << std::endl;
	std::cout << "bad: " << ss.bad() << std::endl;
	std::cout << "eof: " << ss.eof() << std::endl;
	std::cout << "fail: " << ss.fail() << std::endl;
	return (0);
}