#include <opennfs/config_util.h>
#include <stdexcept>
#include <fstream>
#include <stdio.h>

namespace game {

	namespace configuration {
		void ScreenProperties::parse() 
		{
			toolkit::ConfigParser _config("config/gfx");
			_screen.width         = _config.get_int("screen.width");
			_screen.height        = _config.get_int("screen.height");
			_screen.fullscreen    = _config.get_bool("screen.fullscreen");
			_screen.antialiasing  = _config.get_int("screen.antialiasing");
			_screen.depth         = _config.get_int("screen.depth");
			_screen.title         = _config.get_string("screen.title");

		}
		
		types::screen ScreenProperties::get() 
		{
			return _screen;
		}
#define _CINDEX(idx, key) std::string("cloud.layer.") + std::to_string(idx) + std::string(".") + std::string(key)
		types::cloud CloudProperties::parse(int idx)
		{
			toolkit::ConfigParser _config(_prefix + "/config/clouds");
			types::cloud _cloud;
			_cloud.map_name = _config.get_string(_CINDEX(idx, "map"));
			_cloud.scale    = _config.get_float(_CINDEX(idx, "scale"));
			_cloud.altitude = _config.get_float(_CINDEX(idx, "altitude"));
			_cloud.air_humidity = _config.get_float(_CINDEX(idx, "air_humidity"));
			_cloud.position.x = _config.get_float(_CINDEX(idx, "position.x"));
			_cloud.position.z = _config.get_float(_CINDEX(idx, "position.z"));

			return _cloud;
		}
#undef _CINDEX

		void CloudProperties::parse() 
		{
			toolkit::ConfigParser _config(_prefix + "/config/clouds");
			int layer_no = _config.get_int("cloud.layer_no");
			for(int idx=1; idx<=layer_no; idx++) {
				types::cloud _cloud = parse(idx);
				_clouds.push_back(_cloud);
			}
		}
	
		types::all_clouds CloudProperties::get()
		{
			return _clouds;
		}

		int CloudProperties::get_cloud_no()
		{
			return _clouds.size();
		}

		void SkyDomeProperties::parse()
		{
			toolkit::ConfigParser _config(_prefix + "/config/skydome");
			_skydome.exposure = _config.get_float("skydome.exposure");
			_skydome.turbidity = _config.get_float("skydome.turbidity");
			_skydome.sunlight = _config.get_float("skydome.sunlight");
			_skydome.radius = _config.get_float("skydome.radius");
		}
		
		types::skydome SkyDomeProperties::get()
		{
			return _skydome;
		}
	}

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

			if( !is_file.is_open() ) {
				throw std::runtime_error(std::string("can't open file: ") + _config_file );
			}
			std::string line;
			while( std::getline(is_file, line) )
			{
				std::istringstream is_line(line);
				std::string key;
				if( std::getline(is_line, key, '=') )
				{
					std::string value;
					if( std::getline(is_line, value) ) {
						//printf("CFG: %s = %s\n", key.c_str(), value.c_str());
						_cfg.insert(config_param(key, value));
					}
				}
			}
		}
	}
}
