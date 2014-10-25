#include <opennfs/config_util.h>
#include <stdexcept>
#include <fstream>

namespace game {
	namespace toolkit {
		
		std::string ConfigParser::get_string(std::string key) 
		{
			std::string value = _cfg[key];
			if(value == "") {
				throw std::runtime_error("No value for query: ``" + key + "''");
			}
			return value;
		}

		int ConfigParser::get_int(std::string key)
		{
			return std::stoi(get_string(key));
		}

		float ConfigParser::get_float(std::string key)
		{
			return std::stof(get_string(key));
		}

		bool ConfigParser::get_bool(std::string key)
		{
			std::string value = get_string(key);
			if(value == "true" || value == "True" || value == "TRUE") {
				return true;
			}
		
			if(value == "false" || value == "False" || value == "FALSE") {
				return false;
			}

			throw std::runtime_error("boolean can be either true or false!");
		}


		std::string ConfigParser::get_filename()
		{
			return _config_file;
		}

		void ConfigParser::parse()
		{
			std::ifstream is_file(_config_file);

			std::string line;
			while( std::getline(is_file, line) )
			{
				std::istringstream is_line(line);
				std::string key;
				if( std::getline(is_line, key, '=') )
				{
					std::string value;
					if( std::getline(is_line, value) ) {
						_cfg.insert(config_param(key, value));
					}
				}
			}
		}
	}
}
