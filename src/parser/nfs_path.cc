#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "nfs_path.h"

namespace nfs {

std::string GameConfig::to_string() {
	std::string _out;
	for (std::vector<Path*>::iterator it=_config.begin(); it != _config.end(); ++it) {
		Path *p = *it;
		_out +=  std::string("\n") + p->get_name() +  std::string(": ") + p->get_value();
	}

	return _out;
}

void GameConfig::load() {
	fstream cfg_file(path, std::ios::in|std::ios::binary);
	for (std::vector<Path*>::iterator it=_config.begin(); it != _config.end(); ++it) {
		Path *p = *it;
		get_cfg( p->get_name(), cfg_file );
	}
	cfg_file.close();
 
}

#define NFS_CONFIG_PATH_MAX 0x50
void GameConfig::get_cfg(std::string name, fstream &cfg_file) {
	for (std::vector<Path*>::iterator it=_config.begin(); it != _config.end(); ++it) {
		Path *p = *it;
		if ( p->get_name() == name ) {
			cfg_file.position = p->get_offset();
			std::string _value = cfg_file.get_str(NFS_CONFIG_PATH_MAX);
			p->set_value(_value);
		}
	}
}
#undef NFS_CONFIG_PATH_MAX

}

