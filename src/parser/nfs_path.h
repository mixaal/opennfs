#ifndef _NFS_PATH_H
#define _NFS_PATH_H 1

#include <iostream>
#include <string>
#include <vector>
#include "nfs_io.h"

namespace nfs {

class Path {
public:
	Path(std::string name, size_t offset): name(name), offset(offset) {
	}

	size_t get_offset() {
		return offset;
	}
	std::string get_value() {
		return value;
	}
	void set_value(std::string _value) {
		value = _value;
	}	
	std::string get_name() {
		return name;
	}

private:
	std::string name;
	std::string value;
	size_t offset;
};

class GameConfig {
public:
	GameConfig(const std::string path) : path(path) {
		_config.push_back( new Path("game_config", 0) );
		_config.push_back( new Path("save_game", 0x50) );
		_config.push_back( new Path("track_model", 0x2d0) );
		_config.push_back( new Path("track_bitmap", 0x320) );
		load();

        }
        std::string to_string();
private:
	std::vector<Path *> _config; 
	const std::string path;
	void load();
	void get_cfg(std::string name, fstream &cfg_file);
};


}

#endif /* _NFS_PATH_H */
