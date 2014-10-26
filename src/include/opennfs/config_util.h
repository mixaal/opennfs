#ifndef _OPENNFS_CONFIG_UTIL
#define _OPENNFS_CONFIG_UTIL

#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace game {
	namespace configuration {

		namespace types {
			typedef struct {
				int width, height;
				bool fullscreen;
				int antialiasing;
				int depth;
				std::string title;
				int target_fps;
				float field_of_view;
				
			} screen;

			typedef struct {
				float x, y, z;
			} position3d;

			typedef struct {
				std::string map_name;
				float scale;
				float altitude;
				float air_humidity;
				position3d position;
			} cloud;

			typedef std::vector<cloud> all_clouds;

			typedef struct {
				float exposure;
				float turbidity;
				float sunlight;
				float radius;
			} skydome;
		}


		class ScreenProperties {
			public:
				ScreenProperties() {
					parse();
				}
				types::screen get();
			private:
				types::screen _screen;
				void parse();
		};

		class SkyDomeProperties {
			public:
				SkyDomeProperties() {
					_prefix=".";
					parse();
				}
				SkyDomeProperties(std::string prefix) : _prefix(prefix) {
					parse();
				}
				types::skydome get();
			private:
				types::skydome _skydome;
				void parse();
				std::string _prefix;
		};


		class CloudProperties {
			public:
				CloudProperties() {
					_prefix = ".";
					parse();
				}
				CloudProperties(std::string prefix) : _prefix(prefix) {
					parse();
				}
				void parse();
				types::all_clouds get();
				int get_cloud_no();
			private:
				types::all_clouds _clouds;
				types::cloud parse(int idx);
				std::string _prefix;
		};
	}

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
