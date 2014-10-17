/*
 * tools.cc
 *
 *  Created on: 12. 10. 2013
 *      Author: mikc
 */

#include <string.h>
#include <algorithm>
#include <vector>
#include <sstream>
#include <libgen.h>
#include <opennfs/tools.h>

namespace tools {
std::string &StringTools::ltrim(std::string &s) {
	s.erase(s.begin(),
			std::find_if_not(s.begin(), s.end(),
					[](int c) {return isspace(c);}));
	return s;
}

std::string &StringTools::rtrim(std::string &s) {
	s.erase(
			std::find_if_not(s.rbegin(), s.rend(),
					[](int c) {return isspace(c);}).base(), s.end());
	return s;
}

std::string *StringTools::dir_name(std::string &path) {
    const char *d = strdup(dirname(strdup(path.c_str())));
    return new std::string(d);
}

std::string StringTools::file_in_dir(std::string path, std::string fileName) {
    std::string *d = dir_name(path);
    std::cout << "XXX0:" << path << std::endl;
    std::cout << "XXX1:" << fileName << std::endl;
    std::cout << "XXX2:" << *d << std::endl;
    return *d + std::string("/") + fileName;
}

std::string &StringTools::trim(const std::string &s) {
	std::string t = s;
	return ltrim(rtrim(t));
}

bool StringTools::startswith(const std::string &s, const std::string &what) {
	return 0 == s.find(what);
}

std::vector<std::string> StringTools::split(const std::string &s, char delim) {

	std::string segment;
	std::vector<std::string> seglist;
	std::stringstream ss(s);

	while (std::getline(ss, segment, delim)) {
		if (segment.length() > 0) {
			seglist.push_back(segment);
		}
	}
	return seglist;
}

std::vector<float> StringTools::toFloat(const std::string &s, char delim, int pos) {
	std::vector<float> _numbers;
	std::vector<std::string> line = split(s, delim);

	if(pos>=line.size()) return _numbers;

	for (int idx = pos; idx < line.size(); idx++) {
		_numbers.push_back(::atof(line.at(idx).c_str()));
	}
	return _numbers;
}

};

