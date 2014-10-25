#ifndef _OPENNFS_CONFIG_UTIL
#define _OPENNFS_CONFIG_UTIL

#include <string>
#include <sstream>
#include <map>

namespace game {
	namespace toolkit {

		typedef std::pair<std::string, std::string> config_param;

		class ConfigParser {
			public:
				ConfigParser(std::string config_file) 	:  _config_file(config_file) {
					parse();
				}
				std::string get_string(std::string key);
				int get_int(std::string key);
				float get_float(std::string key);
				bool get_bool(std::string key);
				std::string get_filename();

			private:
				std::string _config_file;
				std::map<std::string, std::string> _cfg;
				void parse();
				
		};
	}

}



#endif /* _OPENNFS_CONFIG_UTIL */
