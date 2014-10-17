/*
 * tools.h
 *
 *  Created on: 12. 10. 2013
 *      Author: mikc
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <iostream>

namespace tools {
class StringTools {



public:
	StringTools() { };
	std::string &rtrim(std::string &s);
	std::string &ltrim(std::string &s);
	std::string &trim(const std::string &s);
	std::string *dir_name(std::string &path);
	std::string file_in_dir(std::string path, std::string fileName);
	bool startswith(const std::string &s, const std::string &what);
	std::vector<std::string> split(const std::string &s, char delim=' ');
	std::vector<float> toFloat(const std::string &s, char delim=' ', int pos=0);
};

class MatrixTools {
public:
	MatrixTools() { };
	void print(float *m) {
		std::cout << m[0] <<","<< m[1] <<"," << m[2] << "," << m[3] << std::endl;
		std::cout << m[4] <<","<< m[5] <<"," << m[6] << "," << m[7] << std::endl;
		std::cout << m[8] <<","<< m[9] <<"," << m[10] << "," << m[11] << std::endl;
		std::cout << m[12] <<","<< m[13] <<"," << m[14] << "," << m[15] << std::endl;

	}
};
};




#endif /* TOOLS_H_ */
